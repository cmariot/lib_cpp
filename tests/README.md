# Tests unitaires (framework libunit)

Ce répertoire contient un petit framework de tests (libunit) et des tests spécifiques au projet.

## Objectif

- Permettre d'ajouter rapidement de nouveaux tests unitaires.
- Fournir une cible `make tests` depuis la racine du projet pour compiler et exécuter tous les tests.

## Structure

- `tests/framework/` : code source du framework (libunit). Contient `libunit.a` et le code source pour charger/exécuter les tests.
- `tests/` : tests projet spécifiques :
  - `tests/data_buffer/` : tests pour `DataBuffer`.
  - `tests/pool/` : tests pour `Pool`.
  - `tests/tests_launcher.cpp` : lanceur qui enregistre les tests via `load_test()` et appelle `launch_tests()`.

## Ajouter un test

1. Écrire la logique du test dans un fichier `.cpp` (ex : `tests/my_component/my_component_test.cpp`).
   - Évitez d'utiliser `main()` dans ce fichier : le framework attend des fonctions `int f(void)`.
   - Créez une fonction exportée compatible C (optionnel mais recommandé) :

```cpp
extern "C" int my_component_case1(void) {
    // code de test, assertions, prints...
    return 0; // 0 = OK
}
```

2. Dans `tests/tests_launcher.cpp`, ajoutez une déclaration extern pour votre fonction et appelez `load_test` :

```cpp
extern "C" int my_component_case1(void);

// dans main()
load_test(&tests, "MyComponent", "case1", (void*)my_component_case1, NULL);
```

> Le dernier paramètre (`expected_output`) peut être `NULL` si vous ne voulez pas comparer la sortie standard. Si vous renseignez une chaîne, le framework comparera la sortie capturée au contenu attendu.

3. Depuis la racine du projet, lancez :

```bash
make tests
```

Cela construira `libftpp.a` (la librairie du projet), compilera les tests, liera `tests/framework/libunit.a` et exécutera `bin/run_tests`.

## Exécuter les tests sans recompiler

Pour exécuter le binaire de tests déjà construit :

```bash
make test-only
```

## Nettoyer les artefacts des tests

Pour supprimer le binaire et les objets produits par les tests :

```bash
make tests-clean
```

## Template de test

Tu peux copier ce fichier en base pour créer rapidement de nouveaux tests :

```cpp
// tests/template_test.cpp
#include <iostream>

extern "C" int template_example_test(void) {
    try {
        std::cout << "Hello from template test\n";
        // assertions / checks
        return 0; // OK
    } catch (...) {
        return 255; // KO
    }
}
```

## Conseils

- Préférez des tests indépendants, rapides et déterministes.
- Si un test a besoin d'un comportement RAII (par exemple pour s'assurer qu'une ressource est libérée), enveloppez-le dans un block avec try/catch et retournez 255 en cas d'erreur.
- Documentez chaque test dans son fichier avec un court commentaire indiquant le comportement attendu.

## Support

Si tu veux, je peux :

- ajouter un script pour détecter automatiquement tous les fichiers de tests et générer le launcher,
- ajouter des helpers d'assertion pour uniformiser les messages d'erreur,
- fournir un exemple de test plus complet couvrant des cas limites.

