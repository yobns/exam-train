#!/usr/bin/env bash
# test_filter.sh — batterie de tests exhaustive pour "filter"
# Corrigé: suppression de `set -e` (qui faisait quitter sur ret != 0).

set -u -o pipefail

PROG="${1:-./filter}"
TMPDIR="$(mktemp -d)"
PASS=0
FAIL=0
trap 'rm -rf "$TMPDIR"' EXIT

# Couleurs
G="\033[32m"; R="\033[31m"; Y="\033[33m"; N="\033[0m"

# Vérif programme
if [[ ! -x "$PROG" ]]; then
  echo -e "${Y}[!]${N} Binaire introuvable ou non exécutable: $PROG"
  echo "    Compile: gcc -Wall -Wextra -Werror filter.c -o filter"
  exit 2
fi

# Référence byte-level (remplacement par octets, comme en C)
have_python() { command -v python3 >/dev/null 2>&1; }
have_perl()   { command -v perl    >/dev/null 2>&1; }

expect_ref() {
  local pattern="$1" infile="$2" outfile="$3"
  if have_python; then
    python3 - "$pattern" "$infile" "$outfile" <<'PY'
import sys
pat = sys.argv[1].encode('utf-8','surrogatepass')
data = open(sys.argv[2],'rb').read()
rep = b'*' * len(pat)
open(sys.argv[3],'wb').write(data.replace(pat, rep))
PY
  elif have_perl; then
    PAT="$pattern" perl -0777 -pe '
      binmode(STDIN); binmode(STDOUT);
      my $pat = $ENV{"PAT"}; my $rep = "*" x length($pat);
      s/\Q$pat\E/$rep/g;
    ' <"$infile" >"$outfile"
  else
    echo -e "${R}[X]${N} Besoin de python3 ou perl pour la référence."
    exit 3
  fi
}

show_hex() {
  if command -v hexdump >/dev/null 2>&1; then hexdump -C "$1"; else cat -v -e "$1"; fi
}

run_case() {
  local name="$1" pattern="$2" input="$3" exp_status="$4" use_expect="$5"
  local in="$TMPDIR/in.txt" out="$TMPDIR/out.txt" err="$TMPDIR/err.txt" ref="$TMPDIR/ref.txt"
  printf "%s" "$input" > "$in"

  local ret
  if [[ "$pattern" == "__NOARG__" ]]; then
    "$PROG" <"$in" >"$out" 2>"$err"; ret=$?
  else
    "$PROG" "$pattern" <"$in" >"$out" 2>"$err"; ret=$?
  fi

  local ok=1 msg=""
  [[ "$ret" -eq "$exp_status" ]] || { ok=0; msg+="[ret=$ret≠$exp_status] "; }

  if [[ "$use_expect" == "yes" ]]; then
    expect_ref "$pattern" "$in" "$ref"
    cmp -s "$out" "$ref" || { ok=0; msg+="[stdout diff] "; }
    [[ ! -s "$err" ]] || { ok=0; msg+="[stderr non-vide] "; }
  else
    [[ ! -s "$out" ]] || { ok=0; msg+="[stdout non-vide] "; }
    [[ ! -s "$err" ]] || { ok=0; msg+="[stderr non-vide] "; }
  fi

  if [[ $ok -eq 1 ]]; then
    echo -e "${G}[OK]${N} $name"; PASS=$((PASS+1))
  else
    echo -e "${R}[KO]${N} $name $msg"
    echo "  ├─ input:    $(printf %q "$input")"
    [[ "$pattern" == "__NOARG__" ]] && echo "  ├─ pattern:  (no arg)" || echo "  ├─ pattern:  $(printf %q "$pattern")"
    echo "  ├─ ret:      $ret (exp $exp_status)"
    if [[ "$use_expect" == "yes" ]]; then
      echo "  ├─ diff (stdout):"
      if command -v diff >/dev/null 2>&1; then diff -u "$ref" "$out" | sed 's/^/  │ /' || true
      else echo "  │ expected:"; show_hex "$ref" | sed 's/^/  │ /'; echo "  │ got:"; show_hex "$out" | sed 's/^/  │ /'
      fi
      if [[ -s "$err" ]]; then echo "  └─ stderr:"; show_hex "$err" | sed 's/^/    /'; fi
    else
      if [[ -s "$out" ]]; then echo "  ├─ stdout (should be empty):"; show_hex "$out" | sed 's/^/  │ /'; fi
      if [[ -s "$err" ]]; then echo "  └─ stderr (should be empty):"; show_hex "$err" | sed 's/^/  │ /'; fi
    fi
    FAIL=$((FAIL+1))
  fi
}

echo "==> Testing $PROG"
echo "--- Arguments ---"
run_case "A1 - no arg"              "__NOARG__" ""        1  "no"

# A2: empty arg (explicit)
"$PROG" "" </dev/null >/dev/null 2>"$TMPDIR/errA2"; ret=$?
if [[ $ret -eq 1 && ! -s "$TMPDIR/errA2" ]]; then
  echo -e "${G}[OK]${N} A2 - empty arg"
else
  echo -e "${R}[KO]${N} A2 - empty arg (ret=$ret)"
fi

# A3: un seul argument contenant un espace => valide
run_case "A3 - arg with space"     "a b"       ""        0  "yes"

# A3b: vrais multiples arguments -> doit retourner 1
"$PROG" "a" "b" </dev/null >/dev/null 2>/dev/null; ret=$?
if [[ $ret -eq 1 ]]; then
  echo -e "${G}[OK]${N} A3b - real multiple args"
else
  echo -e "${R}[KO]${N} A3b - real multiple args (ret=$ret≠1)"
fi

echo "--- Entrées vides ---"
run_case "E1 - empty stdin"        "abc"       ""        0  "yes"
run_case "E2 - only newline"       "abc" $'\n'          0  "yes"

echo "--- Cas simples ---"
run_case "S1 - no match"           "zz"        "abcdef"  0  "yes"
run_case "S2 - start match"        "abc"       "abcdef"  0  "yes"
run_case "S3 - middle match"       "bc"        "abcd"    0  "yes"
run_case "S4 - end match"          "def"       "abcdef"  0  "yes"
run_case "S5 - equal to input"     "abc"       "abc"     0  "yes"
run_case "S6 - pat longer"         "abcd"      "abc"     0  "yes"

echo "--- Multi-occurrences ---"
run_case "M1 - repeated spaced"    "ab"        "xxabyyabzz" 0 "yes"
run_case "M2 - adjacent"           "ab"        "ababab"     0 "yes"
run_case "M3 - many"               "abc"       "abcabcabc"  0 "yes"

echo "--- Chevauchements ---"
run_case "O1 - 'aaa' in 'aaaaa'"   "aaa"       "aaaaa"   0  "yes"
run_case "O2 - 'aa' in 'aaaa'"     "aa"        "aaaa"    0  "yes"

echo "--- Multi-lignes ---"
run_case "L1 - across lines"       "ab"  $'ab\nxxab\ny'  0  "yes"
run_case "L2 - empty lines"        "x"   $'\n\nx\n\n'    0  "yes"

echo "--- Espaces & spéciaux ---"
run_case "P1 - pattern with space" "a b"       "a b a  b" 0 "yes"
run_case "P2 - dot literal"        "."         "a.b.c"     0 "yes"
run_case "P3 - brackets literal"   "[x]"       "z[x]z"     0 "yes"
run_case "P4 - backslash"          "\\"        "a\\b\\c"   0 "yes"

echo "--- Newlines & CRLF ---"
run_case "W1 - CRLF pattern"       $'\r\n'     $'a\r\nb\r\nc\r\n' 0 "yes"
run_case "N1 - pattern newline"    $'\n'       $'x\ny\nz\n'       0 "yes"

echo "--- UTF-8 (byte-level) ---"
run_case "U1 - UTF8 é"             "é"         "café périmé"      0 "yes"

if [[ "${FILTER_STRESS:-0}" == "1" ]]; then
  echo "--- Stress (grand input) ---"
  big="$(printf 'a%.0s' {1..120000})"
  run_case "S7 - big input"        "aaa"       "$big"             0 "yes"
fi

if command -v python3 >/dev/null 2>&1; then
  python3 - "$TMPDIR/nul.bin" <<'PY'
import sys
open(sys.argv[1], 'wb').write(b'ab\x00cdabc')
PY
  "$PROG" "abc" <"$TMPDIR/nul.bin" >"$TMPDIR/outN" 2>"$TMPDIR/errN"; ret=$? || true
  echo -e "${Y}[INFO]${N} Test NUL: comportement non spécifié (arrêt au NUL probable). ret=$ret"
fi

echo
echo -e "Résultat: ${G}${PASS} OK${N}, ${R}${FAIL} KO${N}"
[[ $FAIL -eq 0 ]] || exit 1
