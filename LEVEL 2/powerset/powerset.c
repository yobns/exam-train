#include <stdio.h> // Pour printf
#include <stdlib.h> // Pour atoi, malloc, free, exit

// Affiche un sous-ensemble avec espaces entre éléments, \n à la fin
void ft_print_subset(int *subset, int size) {
    int i = 0; // Compteur pour parcourir subset
    while (i < size) { // Boucle sur chaque élément
        printf("%d", subset[i]); // Affiche l'élément courant
        if (i + 1 < size) // Espace si pas le dernier élément
            printf(" ");
        i++; // Passe au suivant
    }
    printf("\n"); // Fin de ligne
}

// Génère récursivement les sous-ensembles, vérifie somme == target
void ft_generate_subsets(int *arr, int size, int target, int *subset, int sub_size, int index, int sum) {
    if (index == size) { // Cas de base : fin du tableau
        if (sum == target && sub_size > 0) // Si somme OK et non-vide
            ft_print_subset(subset, sub_size); // Affiche sous-ensemble
        return; // Fin de cette branche
    }
    // Exclure l'élément courant : avance sans changer subset/sum
    ft_generate_subsets(arr, size, target, subset, sub_size, index + 1, sum);
    // Inclure l'élément courant : ajoute à subset, met à jour sum
    subset[sub_size] = arr[index]; // Stocke élément dans subset
    ft_generate_subsets(arr, size, target, subset, sub_size + 1, index + 1, sum + arr[index]);
}

// Main minimal : parse directement argv, alloue, appelle récursion, libère
int main(int argc, char **argv) {
    if (argc < 2) { // Si pas assez d'args
        printf("\n"); // Affiche ligne vide
        return 0; // Quitte
    }
    int target = atoi(argv[1]); // Convertit 1er arg en target
    int size = argc - 2; // Taille du set (args après target)
    int *arr = malloc(sizeof(int) * size); // Alloue tableau pour input
    if (!arr) // Vérifie erreur malloc
        exit(1); // Quitte avec code 1
    int i = 0; // Compteur pour remplir arr
    while (i < size) { // Parcourt argv[2] à argv[argc-1]
        arr[i] = atoi(argv[i + 2]); // Convertit arg en int
        i++; // Passe au suivant
    }
    int *subset = malloc(sizeof(int) * size); // Alloue subset temporaire
    if (!subset) { // Vérifie erreur malloc
        free(arr); // Libère arr si erreur
        exit(1); // Quitte avec code 1
    }
    ft_generate_subsets(arr, size, target, subset, 0, 0, 0); // Lance récursion
    free(arr); // Libère tableau input
    free(subset); // Libère subset temporaire
    return 0; // Fin
}