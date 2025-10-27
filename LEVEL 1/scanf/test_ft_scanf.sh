#!/usr/bin/env bash
# test_ft_scanf.sh — exhaustive tests for ft_scanf (42), portable BSD/GNU
# Put this script next to ft_scanf.c, then: chmod +x test_ft_scanf.sh && ./test_ft_scanf.sh
set -euo pipefail

CC="${CC:-cc}"
CFLAGS="-std=c99 -O2 -Wall -Wextra -Werror"
TMPDIR="$(mktemp -d)"
trap 'rm -rf "$TMPDIR"' EXIT

GREEN=$'\033[32m'
RED=$'\033[31m'
CYAN=$'\033[36m'
NC=$'\033[0m'

gencode() {
  local outc="$1" use_ft="$2" decls="$3" call="$4" print="$5"
  cat > "$outc" <<EOF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef USE_FT
int ft_scanf(const char *format, ...);
#  define SCANF ft_scanf
#else
#  define SCANF scanf
#endif

int main(void){
    int ret = 0;
    $decls
    $call
    unsigned char rest[512];
    size_t n = fread(rest, 1, sizeof(rest), stdin);
    printf("ret=%d;", ret);
    $print
    printf(";left=");
    for (size_t i=0;i<n;i++) printf("\\\\x%02X", rest[i]);
    printf("\\n");
    return 0;
}
EOF

  local outbin="${outc%.c}"
  if [[ "$use_ft" == "1" ]]; then
    "$CC" $CFLAGS -DUSE_FT "$outc" ft_scanf.c -o "$outbin"
  else
    "$CC" $CFLAGS "$outc" -o "$outbin"
  fi
}

run_case() {
  local name="$1" decls="$2" call_fmt="$3" print_fmt="$4" input_bytes="$5"

  local refc="$TMPDIR/ref_$RANDOM.c" ftc="$TMPDIR/ft_$RANDOM.c"
  gencode "$refc" 0 "$decls" "$call_fmt" "$print_fmt"
  gencode "$ftc"  1 "$decls" "$call_fmt" "$print_fmt"

  local refbin="${refc%.c}" ftbin="${ftc%.c}"
  local out_ref out_ft
  out_ref=$(printf "%b" "$input_bytes" | "$refbin")
  out_ft=$( printf "%b" "$input_bytes" | "$ftbin")

  if [[ "$out_ref" == "$out_ft" ]]; then
    printf "%sOK%s - %s\n" "$GREEN" "$NC" "$name"
  else
    printf "%sFAIL%s - %s\n" "$RED" "$NC" "$name"
    echo "  ref: $out_ref"
    echo "  ft : $out_ft"
  fi
}

echo "${CYAN}== Prebuild ft_scanf.c ==${NC}"
"$CC" $CFLAGS -c ft_scanf.c -o "$TMPDIR/ft_scanf.o"

echo "${CYAN}== Running tests ==${NC}"

########### %d ###########
run_case "%d simple" \
  "int a=0;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $'42'

run_case "%d espaces avant" \
  "int a=0;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $' \t\n  123xyz'

run_case "%d signe +7" \
  "int a=0;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $'+7'

run_case "%d signe -7" \
  "int a=0;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $'-7'

run_case "%d signe seul EOF (aucune assignation)" \
  "int a=111;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $'-'

run_case "%d signe puis lettre (0, rien consommé)" \
  "int a=111;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $'-x'

run_case "%d échec direct (lettre)" \
  "int a=111;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $'abc'

run_case "%d zéros en tête" \
  "int a=0;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $'000123 rest'

########### %s ###########
run_case "%s simple (stop à l'espace)" \
  "char s[64]={0};" \
  "ret = SCANF(\"%s\", s);" \
  "printf(\"s='%s'\", s);" \
  $'hello world'

run_case "%s espaces avant" \
  "char s[64]={0};" \
  "ret = SCANF(\"%s\", s);" \
  "printf(\"s='%s'\", s);" \
  $'   \tfoo\tbar'

run_case "%s seulement whites + EOF (EOF si aucune assignation)" \
  "char s[64]={0};" \
  "ret = SCANF(\"%s\", s);" \
  "printf(\"s='%s'\", s);" \
  $'   \t  '

run_case "%s stop sur tab" \
  "char s[64]={0};" \
  "ret = SCANF(\"%s\", s);" \
  "printf(\"s='%s'\", s);" \
  $'foo\tbar'

run_case "%s stop sur newline" \
  "char s[64]={0};" \
  "ret = SCANF(\"%s\", s);" \
  "printf(\"s='%s'\", s);" \
  $'foo\nbar'

########### %c ###########
run_case "%c lit espace" \
  "char ch=0;" \
  "ret = SCANF(\"%c\", &ch);" \
  "printf(\"c=%d\", (unsigned char)ch);" \
  $' '

run_case "%c lit newline" \
  "char ch=0;" \
  "ret = SCANF(\"%c\", &ch);" \
  "printf(\"c=%d\", (unsigned char)ch);" \
  $'\n'

run_case "\" %c\" (espace dans format => saute whitespace)" \
  "char ch=0;" \
  "ret = SCANF(\" %c\", &ch);" \
  "printf(\"c=%d\", (unsigned char)ch);" \
  $'\nA'

########### LITTÉRAUX / ESPACES DANS FORMAT ###########
run_case "littéral X%dY bon" \
  "int a=0;" \
  "ret = SCANF(\"X%dY\", &a);" \
  "printf(\"d=%d\", a);" \
  $'X42Y'

run_case "littéral mismatch avant conversion (ret=0)" \
  "int a=0;" \
  "ret = SCANF(\"X%dY\", &a);" \
  "printf(\"d=%d\", a);" \
  $'Z42Y'

run_case "\"%d %s\" sans espace effectif" \
  "int a=0; char s[64]={0};" \
  "ret = SCANF(\"%d %s\", &a, s);" \
  "printf(\"d=%d;s='%s'\", a, s);" \
  $'12abc'

run_case "\"%d   %s\" avec gros espaces input" \
  "int a=0; char s[64]={0};" \
  "ret = SCANF(\"%d   %s\", &a, s);" \
  "printf(\"d=%d;s='%s'\", a, s);" \
  $'12      hello'

########### SÉQUENCES ###########
run_case "%d%s%c enchaîné" \
  "int a=0; char s[64]={0}; char ch=0;" \
  "ret = SCANF(\"%d%s%c\", &a, s, &ch);" \
  "printf(\"d=%d;s='%s';c=%d\", a, s, (unsigned char)ch);" \
  $'12helloX'

run_case "%d %d (2e KO => ret=1)" \
  "int a=0,b=0;" \
  "ret = SCANF(\"%d %d\", &a, &b);" \
  "printf(\"a=%d;b=%d\", a, b);" \
  $'12  x'

run_case "%dX%s littéral ok" \
  "int a=0; char s[64]={0};" \
  "ret = SCANF(\"%dX%s\", &a, s);" \
  "printf(\"d=%d;s='%s'\", a, s);" \
  $'12Xabc'

run_case "%dX%s littéral KO" \
  "int a=0; char s[64]={0};" \
  "ret = SCANF(\"%dX%s\", &a, s);" \
  "printf(\"d=%d;s='%s'\", a, s);" \
  $'12Yabc'

########### EOF IMMÉDIAT ###########
run_case "EOF immédiat (aucune donnée) -> ret=EOF" \
  "int a=0;" \
  "ret = SCANF(\"%d\", &a);" \
  "printf(\"d=%d\", a);" \
  $''

########### MÉLANGES FINES ###########
run_case "%s%c collés" \
  "char s[64]={0}; char ch=0;" \
  "ret = SCANF(\"%s%c\", s, &ch);" \
  "printf(\"s='%s';c=%d\", s, (unsigned char)ch);" \
  $'helloXrest'

run_case "%c puis %d (sans saut automatique)" \
  "char ch=0; int a=0;" \
  "ret = SCANF(\"%c%d\", &ch, &a);" \
  "printf(\"c=%d;d=%d\", (unsigned char)ch, a);" \
  $' A9'

run_case "%d puis %c (le %c prend le séparateur)" \
  "int a=0; char ch=0;" \
  "ret = SCANF(\"%d%c\", &a, &ch);" \
  "printf(\"d=%d;c=%d\", a, (unsigned char)ch);" \
  $'42\n'

echo "${CYAN}== Fin des tests ==${NC}"
