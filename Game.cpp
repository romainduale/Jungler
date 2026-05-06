// =====================================================================
// Game.cpp — Fichier principal du jeu JUNGLER
// Il contient toute la logique du jeu : combat, menus, sauvegarde...
// =====================================================================

// On inclut les fichiers "headers" dont on a besoin
// Ces fichiers décrivent les classes qu'on va utiliser
#include "Game.h"       // La classe Game elle-même
#include "Item.h"       // La classe Item (objets d'inventaire)

// On inclut les bibliothèques standard du C++
#include <iostream>     // Pour cout / cin (afficher et lire dans la console)
#include <fstream>      // Pour lire et écrire dans des fichiers (.txt, .csv)
#include <sstream>      // Pour découper des chaînes de caractères facilement
#include <vector>       // Pour utiliser les tableaux dynamiques (vector)
#include <string>       // Pour utiliser les chaînes de texte (string)
#include <cstdlib>      // Pour rand() et exit()
#include <ctime>        // Pour time() → utilisé pour générer des nombres aléatoires
#include <limits>       // Pour numeric_limits (vider le buffer du cin)
#include <direct.h>     // Pour _getcwd() → obtenir le dossier courant (Windows uniquement)

// On dit qu'on utilise l'espace de noms "standard" pour éviter d'écrire std:: partout
using namespace std;


// =====================================================================
// ESPACE DE NOMS : SaveUtils
// Contient des fonctions utilitaires pour la sauvegarde et le chargement
// =====================================================================
namespace SaveUtils {

    // Découpe une ligne de texte en plusieurs morceaux selon un séparateur
    // Exemple : "PLAYER;Alice;100" découpé par ';' → ["PLAYER", "Alice", "100"]
    vector<string> split(const string& line, char sep) {
        vector<string> tokens;  // Le tableau où on va mettre les morceaux
        string token;           // Un morceau à la fois
        stringstream ss(line);  // On transforme la ligne en "flux" pour la lire morceau par morceau
        while (getline(ss, token, sep)) tokens.push_back(token);  // On découpe et on ajoute chaque morceau
        return tokens;  // On retourne le tableau de morceaux
    }

    // Convertit une catégorie (enum) en texte lisible pour la sauvegarder
    // Exemple : MINIBOSS → "MINIBOSS"
    string categoryToString(Category category) {
        if (category == NORMAL) return "NORMAL";
        if (category == MINIBOSS) return "MINIBOSS";
        return "BOSS";  // Si c'est ni NORMAL ni MINIBOSS, c'est forcément BOSS
    }

    // Fait l'inverse : convertit un texte en catégorie (enum)
    // Exemple : "BOSS" → BOSS
    Category stringToCategory(const string& value) {
        if (value == "MINIBOSS") return MINIBOSS;
        if (value == "BOSS") return BOSS;
        return NORMAL;  // Par défaut, si on reconnaît pas, c'est NORMAL
    }
}


// Crée le nom du fichier de sauvegarde à partir du nom du joueur et de son mot de passe
// Exemple : nom="Alice", password="1234" → "save_Alice_1234.txt"
string makeSaveFilename(const string& name, const string& password) {
    return "save_" + name + "_" + password + ".txt";
}


// =====================================================================
// ESPACE DE NOMS : UI
// Contient tout ce qui sert à l'affichage dans la console (couleurs, barres, titres...)
// =====================================================================
#pragma region Affichage_console
namespace UI {

    // Codes couleurs ANSI : ce sont des séquences spéciales qui changent la couleur du texte dans le terminal
    const string RESET = "\033[0m";   // Remet la couleur par défaut
    const string BOLD = "\033[1m";   // Texte en gras
    const string RED = "\033[31m";  // Rouge foncé
    const string GREEN = "\033[32m";  // Vert foncé
    const string YELLOW = "\033[33m";  // Jaune
    const string BLUE = "\033[34m";  // Bleu foncé
    const string MAGENTA = "\033[35m";  // Violet/magenta
    const string CYAN = "\033[36m";  // Cyan foncé
    const string WHITE = "\033[37m";  // Blanc
    const string BRIGHT_RED = "\033[91m";  // Rouge vif
    const string BRIGHT_GREEN = "\033[92m";  // Vert vif
    const string BRIGHT_YELLOW = "\033[93m";  // Jaune vif
    const string BRIGHT_BLUE = "\033[94m";  // Bleu vif
    const string BRIGHT_MAGENTA = "\033[95m";  // Violet vif
    const string BRIGHT_CYAN = "\033[96m";  // Cyan vif

    // Affiche une ligne de séparation longue (====)
    // Le paramètre color est optionnel : par défaut elle est blanche
    void line(const string& color = WHITE) {
        cout << color << "=====================================================" << RESET << endl;
    }

    // Affiche une ligne de séparation courte (----)
    void smallLine(const string& color = WHITE) {
        cout << color << "-----------------------------------------------------" << RESET << endl;
    }

    // Affiche un titre encadré de deux grandes lignes (===)
    // Exemple : title("MENU PRINCIPAL") → affiche un beau titre
    void title(const string& text, const string& color = BRIGHT_GREEN) {
        cout << endl;
        line(color);                                            // Ligne du haut
        cout << color << BOLD << " " << text << RESET << endl; // Le texte du titre
        line(color);                                            // Ligne du bas
    }

    // Affiche un sous-titre encadré de lignes courtes (---)
    void section(const string& text, const string& color = BRIGHT_CYAN) {
        cout << endl;
        smallLine(color);                                       // Ligne du haut
        cout << color << BOLD << " " << text << RESET << endl; // Le texte de la section
        smallLine(color);                                       // Ligne du bas
    }

    // Crée une barre de progression visuelle (pour les HP, Mercy, etc.)
    // Exemple : makeBar(3, 10, 10, BRIGHT_RED) → "[###-------]"
    // value     : valeur actuelle (ex: HP actuel)
    // maxValue  : valeur maximale (ex: HP max)
    // size      : longueur totale de la barre (nombre de cases)
    // fullColor : couleur des cases remplies
    // fullChar  : caractère pour les cases remplies (par défaut '#')
    // emptyChar : caractère pour les cases vides (par défaut '-')
    string makeBar(int value, int maxValue, int size, const string& fullColor, char fullChar = '#', char emptyChar = '-') {
        if (maxValue <= 0) maxValue = 1;  // Évite la division par zéro

        // On calcule combien de cases doivent être remplies (proportionnellement)
        int filled = (value * size) / maxValue;
        if (filled < 0) filled = 0;      // Minimum 0
        if (filled > size) filled = size; // Maximum = taille totale

        string bar = "[";  // On commence la barre

        bar += fullColor;  // On active la couleur
        for (int i = 0; i < filled; ++i) bar += fullChar;    // On ajoute les cases remplies
        bar += RESET;                                          // On remet la couleur normale
        for (int i = filled; i < size; ++i) bar += emptyChar; // On ajoute les cases vides

        bar += "]";  // On ferme la barre
        return bar;
    }
}
#pragma endregion


// =====================================================================
// CONSTRUCTEUR ET DESTRUCTEUR DE LA CLASSE Game
// =====================================================================
#pragma region Constructeur_et_destructeur

// Constructeur : s'exécute quand on crée un objet Game
// On initialise le joueur avec des valeurs par défaut ("Inconnu", 50 HP, 10 ATK, 5 DEF)
Game::Game() : player("Inconnu", 50, 10, 5) {}

// Destructeur : s'exécute quand l'objet Game est détruit (fin du programme)
// On libère la mémoire de tous les monstres créés avec "new" (sinon = fuite mémoire !)
Game::~Game() {
    for (auto m : monsterPool) delete m;  // On supprime chaque monstre de la liste
}
#pragma endregion


// =====================================================================
// SAUVEGARDE ET CHARGEMENT
// Ces fonctions permettent d'écrire la partie dans un fichier et de la relire
// =====================================================================
#pragma region Sauvegarde / Chargement

// Sauvegarde la partie dans un fichier texte
// Retourne true si la sauvegarde a réussi, false sinon
bool Game::saveGame(const string& filename) const {

    // On détermine le nom du fichier à utiliser
    string finalFilename = filename;
    if (finalFilename == "save.txt" && !saveFilename.empty()) {
        finalFilename = saveFilename;  // On utilise le nom personnalisé si dispo
    }

    // On ouvre le fichier en écriture (crée le fichier s'il n'existe pas)
    ofstream file(finalFilename);
    if (!file.is_open()) {  // Si l'ouverture échoue
        cout << UI::BRIGHT_RED << "Impossible de creer le fichier de sauvegarde." << UI::RESET << endl;
        return false;
    }

    // On écrit un "marqueur" au début pour identifier ce fichier comme une sauvegarde valide
    file << "JUNGLER_SAVE_V1" << endl;

    // On écrit les informations du joueur sur une ligne, séparées par des ";"
    file << "PLAYER;" << player.getNom() << ";"
        << player.getHp() << ";" << player.getMaxHp() << ";"
        << player.getAtk() << ";" << player.getDef() << ";"
        << player.getN_victoire() << ";" << player.getN_kill() << ";"
        << player.getN_epargne() << endl;

    // On récupère tous les items de l'inventaire du joueur
    vector<ItemStack> items = player.getInventory().getItems();
    file << "INVENTORY_COUNT;" << items.size() << endl;  // On écrit combien d'items il y a

    // On écrit chaque item : nom, valeur, quantité
    for (const ItemStack& stack : items) {
        Item item = stack.getItem();
        file << "ITEM;" << item.getNom() << ";" << item.getValeur() << ";"
            << stack.getQuantite() << endl;
    }

    // On récupère toutes les entrées du bestiaire
    vector<BestiaryEntry> entries = bestiary.getBestiaire();
    file << "BESTIARY_COUNT;" << entries.size() << endl;  // On écrit combien de monstres ont été vus

    // On écrit chaque monstre du bestiaire
    for (const BestiaryEntry& entry : entries) {
        file << "BEAST;" << entry.getNom() << ";"
            << SaveUtils::categoryToString(entry.getCategory()) << ";"
            << entry.getMaxHp() << ";" << entry.getAtk() << ";"
            << entry.getDef() << ";" << entry.getResultat() << endl;
    }

    file.close();  // On ferme proprement le fichier
    cout << UI::BRIGHT_GREEN << "Partie sauvegardee dans " << finalFilename << " !" << UI::RESET << endl;
    return true;
}

// Charge une partie depuis un fichier texte
// Retourne true si le chargement a réussi, false sinon
bool Game::loadGame(const string& filename) {

    // On ouvre le fichier en lecture
    ifstream file(filename);
    if (!file.is_open()) {  // Si le fichier n'existe pas
        cout << UI::BRIGHT_YELLOW << "Aucune sauvegarde trouvee." << UI::RESET << endl;
        return false;
    }

    // On lit la première ligne pour vérifier que c'est bien une sauvegarde JUNGLER
    string line;
    getline(file, line);
    if (line != "JUNGLER_SAVE_V1") {  // Si ce n'est pas le bon marqueur
        cout << UI::BRIGHT_RED << "Sauvegarde incompatible ou corrompue." << UI::RESET << endl;
        return false;
    }

    // On lit la ligne du joueur et on la découpe
    getline(file, line);
    vector<string> p = SaveUtils::split(line, ';');

    // On vérifie que la ligne a bien 9 champs et commence par "PLAYER"
    if (p.size() != 9 || p[0] != "PLAYER") {
        cout << UI::BRIGHT_RED << "Sauvegarde invalide." << UI::RESET << endl;
        return false;
    }

    // On essaie de reconstruire le joueur depuis les données lues
    // Si quelque chose plante (mauvais format...), on attrape l'erreur avec catch
    try {
        // On extrait chaque info du joueur depuis les morceaux de la ligne
        string nom = p[1];
        int hp = stoi(p[2]);  // stoi = convertit une string en entier
        int maxHp = stoi(p[3]);
        int atk = stoi(p[4]);
        int def = stoi(p[5]);
        int victoires = stoi(p[6]);
        int kills = stoi(p[7]);
        int epargnes = stoi(p[8]);

        // On crée un nouveau joueur avec les stats sauvegardées
        Player loadedPlayer(nom, maxHp, atk, def);

        // Si le joueur n'était pas au max de HP, on lui inflige les dégâts correspondants
        if (hp < maxHp) loadedPlayer.subirDegats(maxHp - hp);

        // On remet les compteurs de victoires, kills et épargnes
        for (int i = 0; i < victoires; ++i) loadedPlayer.add_victoire();
        for (int i = 0; i < kills; ++i)     loadedPlayer.add_kill();
        for (int i = 0; i < epargnes; ++i)  loadedPlayer.add_epargne();

        // --- Chargement de l'inventaire ---
        getline(file, line);
        vector<string> invHeader = SaveUtils::split(line, ';');
        int itemCount = stoi(invHeader[1]);  // Nombre d'items sauvegardés

        // On lit chaque item et on l'ajoute à l'inventaire du joueur chargé
        for (int i = 0; i < itemCount; ++i) {
            getline(file, line);
            vector<string> it = SaveUtils::split(line, ';');
            Item item(it[1], HEAL, stoi(it[2]));  // On recrée l'item (nom, type HEAL, valeur)
            loadedPlayer.getInventory().ajouter_item(item, stoi(it[3]));  // On l'ajoute avec sa quantité
        }

        // On remplace le joueur actuel par le joueur chargé
        player = loadedPlayer;

        // --- Chargement du bestiaire ---
        getline(file, line);
        vector<string> bestiaryHeader = SaveUtils::split(line, ';');
        int bestiaryCount = stoi(bestiaryHeader[1]);  // Nombre de monstres dans le bestiaire

        // On lit chaque monstre du bestiaire et on le remet dans notre bestiaire
        for (int i = 0; i < bestiaryCount; ++i) {
            getline(file, line);
            vector<string> b = SaveUtils::split(line, ';');
            BestiaryEntry entry(
                b[1],                                  // Nom du monstre
                SaveUtils::stringToCategory(b[2]),     // Catégorie (NORMAL / MINIBOSS / BOSS)
                stoi(b[3]),                            // HP max
                stoi(b[4]),                            // Attaque
                stoi(b[5]),                            // Défense
                b[6]                                   // Résultat ("tue" ou "epargne")
            );
            bestiary.ajouter_monstre(entry);
        }
    }
    catch (...) {
        // Si quelque chose s'est mal passé pendant la lecture
        cout << UI::BRIGHT_RED << "Erreur pendant le chargement de la sauvegarde." << UI::RESET << endl;
        return false;
    }

    cout << UI::BRIGHT_GREEN << "Sauvegarde chargee avec succes !" << UI::RESET << endl;
    return true;
}
#pragma endregion


// =====================================================================
// CHARGEMENT DES DONNÉES (ACTs, Items, Monstres)
// Ces fonctions remplissent les listes internes du jeu au démarrage
// =====================================================================
#pragma region Chargement_des_donnees

// Charge toutes les actions (ACTs) disponibles dans le catalogue
// Une ACT = une action spéciale qu'on peut faire en combat pour gagner de la mercy
void Game::loadActs() {
    // On ajoute chaque ACT au catalogue : (identifiant, description, effet sur la mercy)
    actCatalog.addAct(Act("COMPLIMENT", "Vous complimentez la fourrure soyeuse du monstre.", 25));
    actCatalog.addAct(Act("DANSE_TRIBALE", "Vous dansez comme un singe pour amuser le monstre.", 30));
    actCatalog.addAct(Act("OFFRIR_FRUIT", "Vous offrez une mangue juteuse au monstre.", 40));
    actCatalog.addAct(Act("CHATOUILLER", "Vous chatouillez le monstre avec une feuille geante.", 15));
    actCatalog.addAct(Act("INSULTER", "Vous insultez son odeur de jungle humide.", -20));
    actCatalog.addAct(Act("MENACER", "Vous menacez de bruler les lianes autour de lui.", -10));
    actCatalog.addAct(Act("CHANTER", "Vous chantez une melodie apaisante de la jungle.", 20));
    actCatalog.addAct(Act("RACONTER", "Vous racontez une legende absurde sur les totems mayas.", 35));
    actCatalog.addAct(Act("CRIER", "Vous imitez un cri de jaguar enrage.", -15));
    actCatalog.addAct(Act("CARESSER", "Vous caressez gentiment le monstre pour le calmer.", 10));
}

// Charge les items depuis le fichier Items.csv et les ajoute à l'inventaire du joueur
void Game::loadItems() {

    // On affiche le dossier courant pour aider au débogage si le fichier est introuvable
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != nullptr) {
        cout << "\n";
        UI::section("DEBUG - REPERTOIRE DE TRAVAIL", UI::BRIGHT_YELLOW);
        cout << UI::YELLOW << "Repertoire de travail actuel : " << UI::RESET << cwd << endl;
        cout << UI::YELLOW << "Il cherche donc le fichier : " << UI::RESET << cwd << "/Items.csv" << endl;
        UI::smallLine(UI::BRIGHT_YELLOW);
        cout << endl;
    }

    // On ouvre le fichier Items.csv en lecture
    ifstream file("Items.csv");
    if (!file.is_open()) {  // Si le fichier est introuvable
        cout << UI::BRIGHT_RED << "ERREUR : Items.csv introuvable." << UI::RESET << endl;
        cout << UI::RED << "-> Mets le fichier Items.csv exactement dans le dossier ci-dessus !" << UI::RESET << endl;
        exit(1);  // On arrête complètement le programme
    }

    string line;
    int ligneNumero = 0;   // Compteur de lignes (pour afficher les erreurs)
    int itemsCharges = 0;  // Compteur d'items correctement chargés

    // On lit le fichier ligne par ligne
    while (getline(file, line)) {
        ligneNumero++;
        if (line.empty()) continue;  // On ignore les lignes vides

        // On découpe la ligne par ";" pour extraire chaque colonne
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ';')) tokens.push_back(token);

        if (tokens.size() != 4) continue;       // Si pas exactement 4 colonnes → ligne invalide, on passe
        if (tokens[0] == "name") continue;       // On ignore la ligne d'en-tête du CSV

        try {
            string nom = tokens[0];         // Nom de l'item
            int valeur = stoi(tokens[2]);   // Valeur (quantité de soin)
            int quantite = stoi(tokens[3]);  // Quantité en stock

            Item item(nom, HEAL, valeur);                          // On crée l'item (type HEAL = soin)
            player.getInventory().ajouter_item(item, quantite);   // On l'ajoute à l'inventaire
            itemsCharges++;
        }
        catch (...) {
            // Si la conversion échoue (format incorrect), on ignore la ligne et on continue
            cout << UI::BRIGHT_YELLOW << "Ligne " << ligneNumero << " ignoree." << UI::RESET << endl;
        }
    }
    file.close();
    cout << UI::BRIGHT_GREEN << itemsCharges << " items charges avec succes depuis Items.csv !" << UI::RESET << endl;
}

// Charge les monstres depuis le fichier monsters.csv
void Game::loadMonsters() {

    // On ouvre le fichier monsters.csv en lecture
    ifstream file("monsters.csv");
    if (!file.is_open()) {
        cout << UI::BRIGHT_RED << "ERREUR : fichier monsters.csv introuvable. Arret du jeu." << UI::RESET << endl;
        exit(1);  // Le jeu ne peut pas fonctionner sans monstres → on s'arrête
    }

    string line;
    int ligneNumero = 0;
    int monstresCharges = 0;

    // On lit le fichier ligne par ligne
    while (getline(file, line)) {
        ligneNumero++;
        if (line.empty() || line[0] == '#') continue;  // On ignore les lignes vides et les commentaires (commençant par #)

        // On découpe la ligne par ";"
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ';')) tokens.push_back(token);

        if (tokens.size() < 6) {  // Une ligne de monstre a besoin d'au moins 6 colonnes
            cout << UI::BRIGHT_YELLOW << "Ligne " << ligneNumero << " ignoree (trop peu de colonnes)." << UI::RESET << endl;
            continue;
        }

        try {
            // On détermine la catégorie du monstre
            string catStr = tokens[0];
            Category cat;
            if (catStr == "NORMAL")   cat = NORMAL;
            else if (catStr == "MINIBOSS") cat = MINIBOSS;
            else if (catStr == "BOSS")     cat = BOSS;
            else continue;  // Catégorie inconnue → on ignore cette ligne

            // On extrait les stats du monstre
            string nom = tokens[1];
            int hp = stoi(tokens[2]);
            int atk = stoi(tokens[3]);
            int def = stoi(tokens[4]);
            int mercyGoal = stoi(tokens[5]);  // Seuil de mercy nécessaire pour l'épargner

            // On récupère les ACTs disponibles pour ce monstre (jusqu'à 4)
            vector<string> actIds;
            for (size_t i = 6; i < tokens.size() && i < 10; ++i) {
                if (!tokens[i].empty() && tokens[i] != "-") actIds.push_back(tokens[i]);
            }

            // On crée le monstre avec "new" (allocation dynamique) et on l'ajoute au pool
            Monster* m = new Monster(nom, hp, atk, def, cat, mercyGoal, actIds);
            monsterPool.push_back(m);
            monstresCharges++;
        }
        catch (...) {
            cout << UI::BRIGHT_YELLOW << "Ligne " << ligneNumero << " ignoree." << UI::RESET << endl;
        }
    }
    file.close();
    cout << UI::BRIGHT_GREEN << monstresCharges << " monstres charges avec succes depuis monsters.csv !" << UI::RESET << endl;
}
#pragma endregion


// =====================================================================
// COMBAT
// Tout ce qui concerne les batailles contre les monstres
// =====================================================================
#pragma region Combat

// Affiche l'état du combat (barres de HP, mercy, etc.)
void Game::displayBattleStatus(Monster* monster) const {
    cout << "\n";
    UI::section("COMBAT CONTRE " + monster->getNom(), UI::BRIGHT_MAGENTA);

    // Barre de HP du monstre (rouge)
    cout << UI::RED << "HP Monstre : " << UI::RESET
        << UI::makeBar(monster->getHp(), monster->getMaxHp(), 10, UI::BRIGHT_RED)
        << " " << monster->getHp() << "/" << monster->getMaxHp() << endl;

    // Barre de mercy (cyan) → quand elle est pleine on peut épargner le monstre
    cout << UI::CYAN << "Mercy : " << UI::RESET
        << UI::makeBar(monster->getMercy(), monster->getMercyGoal(), 10, UI::BRIGHT_CYAN)
        << " " << monster->getMercy() << "/" << monster->getMercyGoal() << endl;

    // Barre de HP du joueur (verte)
    cout << UI::GREEN << "Votre HP : " << UI::RESET
        << UI::makeBar(player.getHp(), player.getMaxHp(), 10, UI::BRIGHT_GREEN)
        << " " << player.getHp() << "/" << player.getMaxHp() << endl << endl;
}

// Fonction principale du combat : gère le tour par tour jusqu'à ce que le joueur ou le monstre soit vaincu
void Game::battle(Monster* monster) {
    cout << UI::BRIGHT_MAGENTA << ">> Un combat commence contre " << monster->getNom() << " !" << UI::RESET << endl;

    // La boucle tourne tant que les deux combattants sont en vie
    while (player.estEnVie() && monster->estEnVie()) {

        displayBattleStatus(monster);  // On affiche les barres de vie

        // On affiche le menu d'action
        cout << UI::BRIGHT_YELLOW << "+---------------------------------------------+" << UI::RESET << endl;
        cout << UI::BRIGHT_YELLOW << "| 1. FIGHT  2. ACT  3. ITEM  4. MERCY     |" << UI::RESET << endl;
        cout << UI::BRIGHT_YELLOW << "+---------------------------------------------+" << UI::RESET << endl;
        cout << UI::BOLD << "Votre action : " << UI::RESET;

        int action;
        cin >> action;  // On lit le choix du joueur

        // ==================== ACTION 1 : FIGHT (attaquer) ====================
        if (action == 1) {
            cout << UI::BRIGHT_RED << "Vous attaquez !" << UI::RESET << endl;

            // L'attaque inflige un nombre aléatoire de dégâts entre 1 et 70
            int dmg = rand() % 70 + 1;        // ← changer la puissance d'attaque ici
            monster->subirDegats(dmg);
            cout << UI::RED << "Degats infliges : " << dmg << UI::RESET << endl;
        }

        // ==================== ACTION 2 : ACT (action spéciale) ====================
        else if (action == 2) {
            vector<string> acts = monster->getAvailableActs();  // On récupère les ACTs du monstre

            if (acts.empty()) {
                cout << UI::YELLOW << "Aucune ACT disponible." << UI::RESET << endl;
                continue;  // On recommence le tour sans attaque du monstre
            }

            // On affiche la liste des ACTs disponibles
            cout << UI::BRIGHT_CYAN << "Choisissez une ACT :" << UI::RESET << endl;
            for (size_t i = 0; i < acts.size(); ++i) {
                const Act* a = actCatalog.getAct(acts[i]);  // On cherche l'ACT dans le catalogue
                if (a) {
                    cout << UI::CYAN << i + 1 << ". " << a->getTexte() << UI::RESET << endl;
                }
                else {
                    cout << UI::CYAN << i + 1 << ". " << acts[i] << UI::RESET << endl;  // Fallback : on affiche l'identifiant
                }
            }

            int choix;
            cin >> choix;

            // Si le choix est valide
            if (choix >= 1 && choix <= static_cast<int>(acts.size())) {
                string id = acts[choix - 1];
                const Act* a = actCatalog.getAct(id);

                if (a) {
                    cout << UI::BRIGHT_CYAN << a->getTexte() << UI::RESET << endl;
                    // La mercy gagnée est aléatoire : entre -20 et +80
                    int mercyRandom = -20 + (rand() % 101);   // ← MODIFIER ICI la plage de mercy
                    monster->modifier_mercy(mercyRandom);
                }
                else {
                    cout << UI::YELLOW << "ACT non trouvee." << UI::RESET << endl;
                }
            }
        }

        // ==================== ACTION 3 : ITEM (utiliser un objet) ====================
        else if (action == 3) {
            auto& inv = player.getInventory();
            if (inv.estVide()) {
                cout << UI::YELLOW << "Inventaire vide." << UI::RESET << endl;
                continue;  // On revient au début du tour sans que le monstre attaque
            }
            inv.afficher_item();  // On affiche l'inventaire

            cout << UI::BOLD << "Numero de l'item à utiliser : " << UI::RESET;
            int idx;
            cin >> idx;
            player.useItem(idx);  // Le joueur utilise l'item sélectionné
        }

        // ==================== ACTION 4 : MERCY (tenter d'épargner) ====================
        else if (action == 4) {

            if (monster->est_epargne()) {
                // La mercy est suffisante → on épargne le monstre !
                cout << UI::BRIGHT_GREEN << "Vous epargnez le monstre avec succes !" << UI::RESET << endl;
                player.add_epargne();    // On incrémente le compteur d'épargnes
                player.add_victoire();  // On incrémente aussi les victoires

                // On ajoute ce monstre au bestiaire avec le résultat "epargne"
                BestiaryEntry entry(monster->getNom(), monster->getCategory(),
                    monster->getMaxHp(), monster->getAtk(),
                    monster->getDef(), "epargne");
                bestiary.ajouter_monstre(entry);
                return;  // On sort du combat
            }
            else {
                // La mercy est insuffisante → le monstre n'est pas encore prêt
                cout << UI::YELLOW
                    << "Le monstre n'est pas encore pret a etre epargne (Mercy : "
                    << monster->getMercy() << "/" << monster->getMercyGoal() << ")"
                    << UI::RESET << endl;

                // ==================== ATTAQUE DU MONSTRE APRÈS ÉCHEC DE MERCY ====================
                cout << UI::BRIGHT_MAGENTA << monster->getNom() << " attaque !" << UI::RESET << endl;

                int dmg = 0;
                int r = rand() % 100;  // Nombre aléatoire entre 0 et 99

                if (r < 5) {
                    // 5% de chance que le monstre rate complètement
                    cout << UI::YELLOW << "Le monstre rate son attaque !" << UI::RESET << endl;
                }
                else {
                    // 95% des cas : les dégâts sont divisés par 2 (attaque pénalisée)
                    dmg = (rand() % 100 + 1) / 2;   // ← MODIFIER ICI la puissance de base du monstre
                    if (dmg == 0) dmg = 1;           // minimum 1 dégât
                    cout << UI::BRIGHT_RED << "Degats recus : " << dmg << " (moitie)" << UI::RESET << endl;
                }

                if (dmg > 0) {
                    player.subirDegats(dmg);  // On applique les dégâts au joueur
                }
                continue;  // On retourne directement au choix du joueur (sans attaque supplémentaire)
            }
        }

        else {
            cout << UI::BRIGHT_YELLOW << "Choix invalide." << UI::RESET << endl;
            continue;  // On redemande une action sans passer le tour
        }

        // --- Vérification : est-ce que le monstre est mort après l'action du joueur ? ---
        if (!monster->estEnVie()) {
            cout << UI::BRIGHT_GREEN << "Le monstre est vaincu !" << UI::RESET << endl;
            player.add_kill();       // On incrémente les kills
            player.add_victoire();  // Et les victoires

            // On ajoute ce monstre au bestiaire avec le résultat "tue"
            BestiaryEntry entry(monster->getNom(), monster->getCategory(),
                monster->getMaxHp(), monster->getAtk(),
                monster->getDef(), "tue");
            bestiary.ajouter_monstre(entry);
            return;  // On sort du combat
        }

        // ==================== TOUR NORMAL DU MONSTRE (après FIGHT, ACT ou ITEM) ====================
        if (monster->estEnVie()) {
            cout << UI::BRIGHT_MAGENTA << monster->getNom() << " attaque !" << UI::RESET << endl;

            int dmg = 0;
            if (rand() % 100 < 10) {
                // 10% de chance que le monstre rate son attaque normale
                cout << UI::YELLOW << "Le monstre rate son attaque !" << UI::RESET << endl;
            }
            else {
                // 90% des cas : le monstre inflige entre 1 et 50 dégâts
                dmg = rand() % 50 + 1;       // ← MODIFIER ICI la puissance max du monstre en attaque normale
                player.subirDegats(dmg);
                cout << UI::BRIGHT_RED << "Degats reçus : " << dmg << UI::RESET << endl;
            }
        }

        // --- Vérification : le joueur est-il mort après l'attaque du monstre ? ---
        if (!player.estEnVie()) {
            cout << UI::BRIGHT_RED << "Vous avez ete vaincu ! Defaite immediate." << UI::RESET << endl;
            return;  // On sort du combat (défaite)
        }
    }
}

// Choisit un monstre aléatoire dans le pool et démarre un combat contre lui
void Game::startRandomBattle() {
    if (monsterPool.empty()) {
        cout << UI::YELLOW << "Aucun monstre disponible pour le combat." << UI::RESET << endl;
        return;
    }

    // On choisit un index aléatoire dans la liste des monstres
    size_t idx = rand() % monsterPool.size();
    Monster* monster = monsterPool[idx];

    // On remet le monstre à plein HP et à 0 mercy pour que chaque combat soit "frais"
    monster->soigner(monster->getMaxHp());
    monster->modifier_mercy(-monster->getMercy());

    // On annonce l'apparition du monstre
    cout << "\n";
    cout << UI::BRIGHT_MAGENTA << "=====================================================" << UI::RESET << endl;
    cout << UI::BRIGHT_MAGENTA << " Un " << monster->getNom() << " surgit de la jungle !" << UI::RESET << endl;
    cout << UI::BRIGHT_MAGENTA << "=====================================================" << UI::RESET << endl;

    battle(monster);  // On lance le combat
}
#pragma endregion


// =====================================================================
// AFFICHAGES DU JEU (bestiaire, stats, inventaire, fin)
// =====================================================================
#pragma region Affichages_du_jeu

// Affiche le bestiaire : la liste de tous les monstres rencontrés
void Game::showBestiary() const {
    UI::section("BESTIAIRE", UI::BRIGHT_GREEN);

    if (bestiary.estVide()) {
        cout << UI::YELLOW << "Aucun monstre decouvert pour l'instant." << UI::RESET << endl;
        return;
    }

    auto entries = bestiary.getBestiaire();
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];

        // On convertit la catégorie en texte pour l'affichage
        string catStr = (e.getCategory() == NORMAL) ? "NORMAL" :
            (e.getCategory() == MINIBOSS) ? "MINIBOSS" : "BOSS";

        // On affiche le nom et la catégorie du monstre
        cout << UI::BRIGHT_CYAN << i + 1 << ". " << UI::RESET
            << UI::BOLD << e.getNom() << UI::RESET
            << " " << UI::MAGENTA << "(" << catStr << ")" << UI::RESET << endl;

        // On affiche ses stats
        cout << " " << UI::RED << "HP max : " << UI::RESET << e.getMaxHp()
            << " | " << UI::YELLOW << "ATK : " << UI::RESET << e.getAtk()
            << " | " << UI::BLUE << "DEF : " << UI::RESET << e.getDef() << endl;

        // On affiche si le monstre a été tué ou épargné
        cout << " " << UI::GREEN << "Resultat : " << UI::RESET << e.getResultat() << endl << endl;
    }
}

// Affiche les statistiques du joueur
void Game::showStats() const {
    UI::section("STATISTIQUES", UI::BRIGHT_BLUE);
    cout << UI::CYAN << "Nom : " << UI::RESET << player.getNom() << endl;
    cout << UI::GREEN << "HP : " << UI::RESET << player.getHp() << " / " << player.getMaxHp() << endl;
    cout << UI::RED << "Attaque : " << UI::RESET << player.getAtk() << endl;
    cout << UI::BLUE << "Defense : " << UI::RESET << player.getDef() << endl;
    cout << UI::YELLOW << "Victoires : " << UI::RESET << player.getN_victoire() << " / 10" << endl;
    cout << UI::BRIGHT_RED << "Tues : " << UI::RESET << player.getN_kill() << endl;
    cout << UI::BRIGHT_GREEN << "Epargnes : " << UI::RESET << player.getN_epargne() << endl;
}

// Affiche l'inventaire et permet d'utiliser un item hors combat
void Game::showItems() {
    UI::section("INVENTAIRE", UI::BRIGHT_YELLOW);
    auto& inv = player.getInventory();

    if (inv.estVide()) {
        cout << UI::YELLOW << "Votre sac est vide." << UI::RESET << endl;
        return;
    }

    inv.afficher_item();  // On liste tous les items

    cout << "\n" << UI::GREEN << "Tous ces items soignent des HP." << UI::RESET << endl;
    cout << UI::BOLD << "Numero de l'item a utiliser (ou -1 pour retour) : " << UI::RESET;
    int choix;
    cin >> choix;

    if (choix >= 0) player.useItem(choix);  // Si le joueur ne choisit pas -1, on utilise l'item
}

// Affiche la fin du jeu avec l'ending correspondant aux choix du joueur
void Game::showEnding() const {
    cout << "\n";
    UI::title("FIN DE L'AVENTURE", UI::BRIGHT_MAGENTA);

    int kills = player.getN_kill();
    int spared = player.getN_epargne();

    // On choisit l'ending selon les stats du joueur
    if (kills == 10) {
        // Toutes les victoires = tuer → fin génocidaire
        cout << UI::BRIGHT_RED << "FIN GENOCIDAIRE : Vous avez extermine toute la jungle !" << UI::RESET << endl;
    }
    else if (spared == 10) {
        // Toutes les victoires = épargner → fin pacifiste
        cout << UI::BRIGHT_GREEN << "FIN PACIFISTE : Vous avez apaise la jungle avec ingeniosite !" << UI::RESET << endl;
    }
    else {
        // Mélange des deux → fin neutre
        cout << UI::BRIGHT_YELLOW << "FIN NEUTRE : Un equilibre entre force et compassion." << UI::RESET << endl;
    }

    // On affiche les stats finales
    cout << UI::RED << "Tues : " << kills << UI::RESET << endl;
    cout << UI::GREEN << "Epargnes : " << spared << UI::RESET << endl;
    cout << UI::BRIGHT_CYAN << "Merci d'avoir joue a JUNGLER !" << UI::RESET << endl;
}
#pragma endregion


// =====================================================================
// MENU PRINCIPAL
// Affiché en boucle pendant toute la partie
// =====================================================================
#pragma region Menu_principal

// Affiche le menu principal et traite le choix du joueur
void Game::showMainMenu() {
    cout << "\n";
    UI::section("MENU PRINCIPAL", UI::BRIGHT_GREEN);

    // On affiche les options disponibles
    cout << UI::BRIGHT_RED << "1. " << UI::RESET << "Demarrer un combat" << endl;
    cout << UI::BRIGHT_CYAN << "2. " << UI::RESET << "Bestiaire" << endl;
    cout << UI::BRIGHT_BLUE << "3. " << UI::RESET << "Statistiques" << endl;
    cout << UI::BRIGHT_YELLOW << "4. " << UI::RESET << "Items" << endl;
    cout << UI::BRIGHT_GREEN << "5. " << UI::RESET << "Sauvegarder" << endl;
    cout << UI::BRIGHT_MAGENTA << "6. " << UI::RESET << "Quitter" << endl;
    cout << UI::BOLD << "Votre choix : " << UI::RESET;

    int choix;
    cin >> choix;

    // On exécute l'action correspondante
    switch (choix) {
    case 1:
        // On vérifie que le joueur est en vie avant de se battre
        if (!player.estEnVie()) {
            cout << UI::YELLOW << "Vous etes a 0 HP ! Utilisez un item pour vous soigner avant de combattre." << UI::RESET << endl;
        }
        else {
            startRandomBattle();  // On lance un combat aléatoire
        }
        break;
    case 2: showBestiary(); break;    // On affiche le bestiaire
    case 3: showStats();    break;    // On affiche les stats
    case 4: showItems();    break;    // On affiche l'inventaire
    case 5: saveGame();     break;    // On sauvegarde la partie
    case 6:
        saveGame();  // On sauvegarde avant de quitter
        cout << UI::BRIGHT_CYAN << "Au revoir, exploratrice !" << UI::RESET << endl;
        exit(0);  // On ferme le programme proprement
    default:
        cout << UI::BRIGHT_YELLOW << "Choix invalide." << UI::RESET << endl;
    }
}
#pragma endregion


// =====================================================================
// LANCEMENT DU JEU
// C'est le point d'entrée : tout commence ici
// =====================================================================
#pragma region Lancement_du_jeu

// Démarre le jeu : initialise tout et gère le menu de départ (nouvelle partie / charger)
void Game::start() {

    // On initialise le générateur de nombres aléatoires avec l'heure actuelle
    // → ça garantit que les nombres seront différents à chaque lancement
    srand(static_cast<unsigned>(time(nullptr)));

    // On affiche le titre du jeu
    cout << UI::BRIGHT_GREEN << "======================================" << UI::RESET << endl;
    cout << UI::BRIGHT_GREEN << " BIENVENUE DANS JUNGLER " << UI::RESET << endl;
    cout << UI::BRIGHT_GREEN << "======================================" << UI::RESET << endl;

    // On charge les monstres et les ACTs depuis les fichiers CSV
    loadMonsters();
    loadActs();

    // On demande au joueur s'il veut une nouvelle partie ou charger une existante
    cout << "\n1. Nouvelle partie" << endl;
    cout << "2. Charger la sauvegarde" << endl;
    cout << "Votre choix : ";
    int choixDepart = 1;
    cin >> choixDepart;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // On vide le buffer pour que getline() fonctionne après

    bool loaded = false;
    string nom, password;

    if (choixDepart == 2) {
        // --- Chargement d'une partie existante ---
        cout << "Nom de l'exploratrice : ";
        getline(cin, nom);
        cout << "Mot de passe : ";
        getline(cin, password);
        saveFilename = makeSaveFilename(nom, password);  // On reconstitue le nom du fichier de sauvegarde
        loaded = loadGame(saveFilename);                 // On essaie de charger
    }

    if (!loaded) {
        // --- Nouvelle partie (ou chargement échoué) ---
        cout << "Nom de votre exploratrice : ";
        getline(cin, nom);
        if (nom.empty()) nom = "Exploratrice";  // Nom par défaut si l'utilisateur ne saisit rien

        cout << "Choisissez un mot de passe : ";
        getline(cin, password);

        saveFilename = makeSaveFilename(nom, password);  // On crée le nom du fichier de sauvegarde

        // On crée un joueur tout neuf avec ses stats de départ
        player = Player(nom, 100, 20, 10);

        loadItems();              // On charge les items dans l'inventaire
        saveGame(saveFilename);  // On sauvegarde immédiatement la nouvelle partie
    }

    // On affiche un résumé de la situation au début
    cout << "\n";
    UI::section("RESUME DE L'EXPEDITION", UI::BRIGHT_CYAN);
    cout << UI::CYAN << "Nom : " << UI::RESET << player.getNom() << endl;
    cout << UI::GREEN << "HP : " << UI::RESET << player.getHp() << "/" << player.getMaxHp() << endl;
    cout << UI::YELLOW << "Inventaire initial :" << UI::RESET << endl;

    if (player.getInventory().estVide()) {
        cout << " " << UI::YELLOW << "(vide)" << UI::RESET << endl;
    }
    else {
        player.getInventory().afficher_item();
    }

    cout << "\n" << UI::BRIGHT_MAGENTA << "L'aventure commence..." << UI::RESET << "\n";

    // Boucle principale du jeu : on affiche le menu jusqu'à ce que le joueur ait fait 10 combats
    while (true) {
        if (player.getN_victoire() >= 10) {
            showEnding();  // Le joueur a terminé ses 10 combats → on affiche la fin
            break;         // On sort de la boucle → fin du jeu
        }
        showMainMenu();  // Sinon on affiche le menu principal
    }
}
#pragma endregion
