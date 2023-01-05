#include <iostream>
#include <string>
using namespace std;

struct BTreeNode {
    int n;                    // liczba kluczy
    int* keys;                // tablica kluczy
    int t;                    // minimalny stopień b-drzewa
    bool leaf;                // prawda - węzeł jest liściem, fałsz - węzeł nie jest liściem
    BTreeNode** children;     // dzieci konkretnego węzła
};

struct BTree {
    int t;                    // minimalny stopień b-drzewa
    BTreeNode* root;          // węzeł korzeń b-drzewa
};

BTree* allocateTree(int t) {                       // alokowanie nowego drzewa 
    BTree* newTree = new BTree;
    newTree->t = t;
    newTree->root = nullptr;
    return newTree;
}

BTreeNode* allocateNode(int t, bool leaf) {        // alokowanie nowego węzła
    BTreeNode* node = new BTreeNode;
    node->t = t;
    node->leaf = leaf;
    node->keys = new int[2 * t - 1];               // ustawianie maksymalnej liczby kluczy na 2t - 1
    node->children = new BTreeNode * [2 * t];      // ustawianie maksymalnej liczby dziecki na 2t
    for (int i = 0; i < 2 * t; i++) {
        node->children[i] = nullptr;
    }
    node->n = 0;
    return node;
}

void printNode(BTreeNode* node) {                  // wypisywanie zawartości węzłów b-drzewa
    int i;
    for (i = 1; i < node->n + 1; i++) {
        if (node->leaf != true) {                  // jeśli węzeł nie jest liściem wywołujemy funkcję dla jego dziecka
            printNode(node->children[i]);
        }
        cout << node->keys[i] << " ";
    }

    if (node->leaf != true) {                      // jeśli węzeł nie jest liściem wywołujemy funkcję dla jego dziecka
        printNode(node->children[i]);
    }
}

void printTree(BTree* tree) {                      // wypisywanie zawartości b-drzewa
    if (tree->root != nullptr) {                   // sprawdzamy, czy drzewo zostało już zaalokowane
        printNode(tree->root); 
    }
    cout << endl;
}

void saveNode(BTreeNode* node) {                   // wypisywanie zawartości węzłów b-drzewa przy zachowniu wymaganej struktury
    int i;
    cout << "( ";
    for (i = 1; i < node->n + 1; i++) {
        if (node->leaf != true) {                  // jeśli węzeł nie jest liściem wywołujemy funkcję dla jego dziecka
            saveNode(node->children[i]);
        }
        cout << node->keys[i] << " ";
    }
 
    if (node->leaf != true) {                      // jeśli węzeł nie jest liściem wywołujemy funkcję dla jego dziecka
        saveNode(node->children[i]);
    }
    cout << ") ";
}

void saveTree(BTree* tree) {                       // wypisywanie zawartości b-drzewa przy zachowniu wymaganej struktury
    cout << tree->root->t << endl;
    if (tree->root != nullptr) {
        saveNode(tree->root);
    }
    cout << endl;
}

void bTreeSplitChild(BTreeNode* parent, int i, BTreeNode* child) {         // rozbijanie węzła dziecka rodzica w b-drzewie
    BTreeNode* newNode = allocateNode(child->t, child->leaf);              // dodajemy nowy węzeł, który będzie przechowywał t - 1 kluczy rodzica
    newNode->n = parent->t - 1;

    for (int j = 1; j <= parent->t - 1; j++) {                             // kopiujemy ostatnie t - 1 kluczy rodzica do nowego węzła
        newNode->keys[j] = child->keys[j + parent->t];
    }
    if (child->leaf != true) {                                             // jeśli dziecko nie jest liściem, kopiuj ostatnie t dzieci rodzica do nowego węzła
        for (int j = 1; j <= parent->t; j++) {
            newNode->children[j] = child->children[j + parent->t];
        }
    }
    child->n = parent->t - 1;                                              // po przekopiowaniu zmniejszamy liczbę kluczy rodzica do t - 1 
    for (int j = parent->n + 1; j >= i + 1; j--) {                         // tworzymy miejsce w węźle na nowe dziecko, czyli nasz nowy węzeł
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = newNode;                                     // dodajemy nowy węzeł na stworzonege miejsce

    for (int j = parent->n; j >= i; j--) {                                 // klucz z rodzica również zostanie przeniesiony, dlatego musimy przesunąć kluczę, aby wypełnić lukę
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = child->keys[parent->t];                              // kopiujemy liczbę kluczy dziecka do rodzica
    parent->n = parent->n + 1;                                             // zwiększamy liczbę kluczy rodzica o jeden
}

void bTreeInsertNonfull(BTreeNode* node, int key) {                        // funckja wstawia klucz do drzewa o niepełnym korzeniu, schodzi rekurencyjnie w dół drzewa dbając, aby węzeł do którego schodzi był niepełny
    int i = node->n;                                                       
    if (node->leaf != false) {                                             // gdy węzeł jest liściem, umieszczamy w nim klucz
        while ((i >= 1) && (key < node->keys[i])) {
            node->keys[i + 1] = node->keys[i];
            i -= 1;
        }
        node->keys[i + 1] = key;
        node->n = node->n + 1;
    }
    else {                                                                // gdy węzeł nie jest liściem, klucz musi zostać wstawiony do odpowiedniego liścia w poddrzewie o korzeniu w aktualnym węźle
        while ((i >= 1) && (key < node->keys[i])) {                       // ustawiamy indeks dziecka, które będzie miało nowy klucz
            i -= 1;
        }
        i += 1;                                                           // wyrównujemy indeks, gdyż indeksujemy od 1
        if (node->children[i]->n == 2 * node->t - 1) {                    // sprawdzamy czy ilość kluczy dziecka o wyznaczonym indeksie jest maksymalna 
            bTreeSplitChild(node, i, node->children[i]);                  // rozdzielamy pełny węzeł
            if (key > node->keys[i]) {                                    // po podziale środkowy klucz dziecka zostaje przeniesiony w górę drzewa, a samo dziecko zostaje podzielone na dwa węzły 
                i += 1;                                                   // sprawdzamy w którym z dwóch dzieci powinniśmy umieścić klucz
            }
        }
        bTreeInsertNonfull(node->children[i], key);                       // wywołujemy funkcję dodającą klucz do dziecka o wyznacoznym indeksie, gdy węzeł nie jest pełny
    }
}

void bTreeInsert(BTree* tree, int key) {                                  // funckja dodająca klucz do b-drzewa
    if (tree->root != nullptr) {                                          // sprawdzamy czy drzewo zostało już zaalokowane i posiada korzeń
        if (tree->root->n == 2 * tree->t - 1) {                           // sprawdzamy czy korzeń jest pełny, jeśli tak drzewo musi rosnąć w dół
            BTreeNode* twoSonsNode = allocateNode(tree->t, false);        // tworzymy węzeł, który zostanie naszym nowym wkorzeniem
            twoSonsNode->children[1] = tree->root;                        // ustawiamy poprzedni korzeń jako jego pierwsze dziecko
            bTreeSplitChild(twoSonsNode, 1, tree->root);                  // dzielimy stary węzeł i dodajemy do nowego korzenia jednek klucz
            bTreeInsertNonfull(twoSonsNode, key);                         // dodajemy do nowego korzenia klucz
            tree->root = twoSonsNode;                                     // ostatecznie ustawiamy korzeń b-drzewa na nasz nowo utoworzony węzeł
        }
        else {
            bTreeInsertNonfull(tree->root, key);                          // jeśli korzeń nie jest pełen wywołujemy funkcję dla dodania do niepełnego węzęzła
        } 
    }
    else {                                                                // jeśli korzeń jest pusty tworzymy węzeł który zostanie naszym korzeniem, dodajemy do niego klucz i ustalamy ilość kluczy na 1
        tree->root = allocateNode(tree->t, true);
        tree->root->keys[1] = key;
        tree->root->n = 1;
    }
}

void bTreeSearch(BTreeNode* node, int key) {                              // przeszukiwanie b-drzewa
    int i = 1;
    while ((i <= node->n) && (key > node->keys[i])) {                     // wyznaczamy indeks pierwszego klucza większego niż szukany klucz
        i += 1;
    }
    if ((i <= node->n) && (key == node->keys[i])) {                       // jeśli indeks mieści się w liczbie kluczy i klucz szukany i wyznaczony przez indeks są identyczne znaleziono klucz
        cout << key << " +" << endl;
        return;
    }
    if (node->leaf != false) {                                            // jeśli znajdujemy się w liście, nie znaleźliśmy klucza w drzewie
        cout << key << " -" << endl;
    }
    else {
        bTreeSearch(node->children[i], key);                              // jeśli nie jesteśmy w liściu i nie znaleźliśmy jeszcze klucza schodzimy głębiej w poszukiwaniu klucza, rozpoczynamy od dzieci obecnego węzła
        return;
    }
}

void check(BTreeNode* &node) {                                            // sprawdzamy jaki znak wczytujemy i wykonujemy odpowiednie operacje w celu stworzenia drzewa
    string mark; 
    int child = 1;                                                        // ustawiamy startowy indeks pierwszego dziecka
    while (cin >> mark) {                                                 // wczytujemy znak
        if (mark == ")") {                                                // nawias zamykający oznacza powrót to węzła rodzica 
            return;
        }
        else if ((mark[0] > 47) && (mark[0] < 58)) {                      // jeśli otrzymujemy liczbę, oznacza to, że musimy dodać klucze do węzła w którym się znajdujemy
            node->keys[node->n + 1] = stoi(mark);
            node->n += 1;
        }
        else if (mark == "(") {                                           // nawias otwierający oznacza rozpoczęscie nowego węzła jako dziecko węzła w którym się znajdujemy
            node->leaf = false;                                           // węzeł nie będzie już dłużej liściem
            node->children[child] = allocateNode(node->t, true);          // dodajemy nowy węzeł jako dziecko węzła w którym aktualnie się znajdujemy
            check(node->children[child]);                                 // dokonujemy dalszego sprawdzania wczytywanych znaków, tylko w niższym węźle
            child += 1;
        }
    }
}

void bTreeLoad(BTree* &tree, int t) {                                     // ładujemy nowe drzewa przy pomocy zczytania jego wartości i układu z konsoli
    string mark;
    cin >> mark;                                                          // wczytujemy pierwszy znak

    tree = allocateTree(t);                                               // alokujemy nowe drzewo

    tree->root = allocateNode(tree->t, true);                             // alokujemy konrzeń nowego drzewa
    tree->root->n = 0;                                                    // ustawiamy ilość klcuzy na 0

    if (mark == "(") {                                                    // jeśli pierwszy znak jest otwierającym nazwiasek rozpoczynamy rekurencyjną procedurę wczytywania i sprawdzania kolejnych znaków z konsoli
        check(tree->root);
    }
}

void freeNode(BTreeNode* node) {                                          // usuwamy kolejne węzły drzewa z pamięci, zaczynając od węzłów najniższych
    int i;
    for (i = 1; i < node->n + 1; i++) {
        if (node->leaf != true) {
            freeNode(node->children[i]);
        }
    }

    if (node->leaf != true) {
        freeNode(node->children[i]);
    }
    else {
        for (i = 1; i < node->n + 1; i++) {
            delete node;
        }
    }
}

void freeTree(BTree* tree) {                                              // usuwamy skonstruowane drzewo z pamięci
    if (tree->root != nullptr) {
        freeNode(tree->root);
        delete tree;
    }
}

int main() {
    BTree* tree = nullptr;

    string command;
    cin >> command;
    while (command != "X") {
        if (command == "I") {
            int x;
            cin >> x;

            tree = allocateTree(x);
        }
        else if (command == "A") {
            int x;
            cin >> x;

            if (tree != nullptr) {
                bTreeInsert(tree, x);
            }
            else {
                cout << endl;
            }          
        }
        else if (command == "?") {
            int x;
            cin >> x;

            if (tree != nullptr) {
                bTreeSearch(tree->root, x);
            }
            else {
                cout << endl;
            }
        }
        else if (command == "P") {
            if (tree != nullptr) {
                printTree(tree);
            }
            else {
                cout << endl;
            }    
        }
        else if (command == "L") {
            int x;
            cin >> x;
            
            bTreeLoad(tree, x);
        }
        else if (command == "S") {
            if (tree != nullptr) {
                saveTree(tree);
            }
            else {
                cout << endl;
            }
        }
       /*else if (command == "F") {
            freeTree(tree);
            BTree* tree = nullptr;
        }*/
        cin >> command;
    }
    freeTree(tree);
}