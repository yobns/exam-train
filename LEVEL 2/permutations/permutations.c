#include <stdlib.h> // Inclusion de la bibliothèque pour malloc, calloc, free
#include <unistd.h> // Inclusion pour la fonction write
#include "permutations.h" // Inclusion du fichier d'en-tête (non fourni, probablement pour déclarations)

// Affiche une permutation suivie d'un retour à la ligne
static void ft_print_permutation(char *perm, int size) {
    write(1, perm, size); // Écrit la permutation de taille 'size' sur la sortie standard (stdout, file descriptor 1)
    write(1, "\n", 1); // Écrit un caractère de nouvelle ligne pour séparer les permutations
}

// Trie la chaîne en place en utilisant l'algorithme de tri à bulles
void ft_sort_string(char *str, int size) {
    int i = 0; // Compteur pour la boucle externe
    while (i < size - 1) { // Parcourt jusqu'à l'avant-dernier élément
        int j = 0; // Compteur pour la boucle interne
        while (j < size - i - 1) { // Compare les paires adjacentes jusqu'à la fin non triée
            if (str[j] > str[j + 1]) { // Si l'élément courant est supérieur au suivant
                char tmp = str[j]; // Stocke l'élément courant dans une variable temporaire
                str[j] = str[j + 1]; // Échange : place l'élément suivant dans la position courante
                str[j + 1] = tmp; // Échange : place l'élément temporaire dans la position suivante
            }
            j++; // Passe à la paire suivante
        }
        i++; // Réduit la taille de la partie non triée
    }
}

// Génère toutes les permutations possibles de la chaîne de manière récursive
void ft_generate_permutations(char *arr, int size, char *perm, int pos, int *used) {
    if (pos == size) { // Cas de base : si la position atteint la taille, la permutation est complète
        ft_print_permutation(perm, size); // Affiche la permutation complète
        return; // Termine la récursion
    }
    int i = 0; // Compteur pour parcourir les caractères
    while (i < size) { // Parcourt chaque caractère de la chaîne triée
        if (!used[i]) { // Si le caractère à l'index i n'a pas encore été utilisé
            used[i] = 1; // Marque le caractère comme utilisé dans le tableau de flags
            perm[pos] = arr[i]; // Ajoute le caractère à la permutation en cours à la position pos
            ft_generate_permutations(arr, size, perm, pos + 1, used); // Appel récursif pour la position suivante
            used[i] = 0; // Backtracking : démarcage le caractère pour permettre d'autres combinaisons
        }
        i++; // Passe au caractère suivant
    }
}

// Point d'entrée du programme
int main(int argc, char **argv) {
    if (argc != 2) { // Vérifie qu'exactement un argument (la chaîne) est passé avec le nom du programme
        write(1, "\n", 1); // Si l'argument est absent ou multiple, affiche une ligne vide
        return 0; // Termine le programme
    }
    int size = 0; // Variable pour stocker la longueur de la chaîne d'entrée
    while (argv[1][size]) // Calcule la longueur de la chaîne en parcourant jusqu'au caractère nul (\0)
        size++;
    if (size == 0) { // Si la chaîne est vide
        write(1, "\n", 1); // Affiche une ligne vide
        return 0; // Termine le programme
    }
    char *arr = malloc(size); // Alloue un tableau pour copier la chaîne d'entrée
    char *perm = malloc(size); // Alloue un tableau pour construire les permutations
    int *used = calloc(size, sizeof(int)); // Alloue un tableau de flags initialisé à 0 pour suivre les caractères utilisés
    if (!arr || !perm || !used) // Vérifie si les allocations mémoire ont réussi
        return 1; // Si une allocation échoue, termine avec une erreur
    int i = 0; // Compteur pour copier la chaîne
    while (i < size) { // Copie chaque caractère de l'argument dans arr
        arr[i] = argv[1][i]; // Copie caractère par caractère
        i++;
    }
    ft_sort_string(arr, size); // Trie la chaîne copiée par ordre alphabétique pour garantir l'ordre lexicographique
    ft_generate_permutations(arr, size, perm, 0, used); // Génère et affiche toutes les permutations
    free(arr); // Libère la mémoire allouée pour la copie de la chaîne
    free(perm); // Libère la mémoire allouée pour les permutations temporaires
    free(used); // Libère la mémoire allouée pour le tableau de flags
    return 0; // Termine le programme avec succès
}