// Game.cpp
#include "Game.h"
#include "Item.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <direct.h>

using namespace std;

#pragma region Couleurs_et_affichage_console
namespace UI {
    const string RESET = "\033[0m";
    const string BOLD = "\033[1m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";
    const string WHITE = "\033[37m";
    const string BRIGHT_RED = "\033[91m";
    const string BRIGHT_GREEN = "\033[92m";
    const string BRIGHT_YELLOW = "\033[93m";
    const string BRIGHT_BLUE = "\033[94m";
    const string BRIGHT_MAGENTA = "\033[95m";
    const string BRIGHT_CYAN = "\033[96m";

    void line(const string& color = WHITE) {
        cout << color << "=====================================================" << RESET << endl;
    }
    void smallLine(const string& color = WHITE) {
        cout << color << "-----------------------------------------------------" << RESET << endl;
    }
    void title(const string& text, const string& color = BRIGHT_GREEN) {
        cout << endl;
        line(color);
        cout << color << BOLD << " " << text << RESET << endl;
        line(color);
    }
    void section(const string& text, const string& color = BRIGHT_CYAN) {
        cout << endl;
        smallLine(color);
        cout << color << BOLD << " " << text << RESET << endl;
        smallLine(color);
    }
    string makeBar(int value, int maxValue, int size, const string& fullColor, char fullChar = '#', char emptyChar = '-') {
        if (maxValue <= 0) maxValue = 1;
        int filled = (value * size) / maxValue;
        if (filled < 0) filled = 0;
        if (filled > size) filled = size;
        string bar = "[";
        bar += fullColor;
        for (int i = 0; i < filled; ++i) bar += fullChar;
        bar += RESET;
        for (int i = filled; i < size; ++i) bar += emptyChar;
        bar += "]";
        return bar;
    }
}
#pragma endregion

#pragma region Constructeur_et_destructeur
Game::Game() : player("Inconnu", 50, 10, 5) {}
Game::~Game() {
    for (auto m : monsterPool) delete m;
}
#pragma endregion

#pragma region Chargement_des_donnees
void Game::loadActs() {
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

void Game::loadItems() {
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != nullptr) {
        cout << "\n";
        UI::section("DEBUG - REPERTOIRE DE TRAVAIL", UI::BRIGHT_YELLOW);
        cout << UI::YELLOW << "Repertoire de travail actuel : " << UI::RESET << cwd << endl;
        cout << UI::YELLOW << "Il cherche donc le fichier : " << UI::RESET << cwd << "/Items.csv" << endl;
        UI::smallLine(UI::BRIGHT_YELLOW);
        cout << endl;
    }
    ifstream file("Items.csv");
    if (!file.is_open()) {
        cout << UI::BRIGHT_RED << "ERREUR : Items.csv introuvable." << UI::RESET << endl;
        cout << UI::RED << "→ Mets le fichier Items.csv exactement dans le dossier ci-dessus !" << UI::RESET << endl;
        exit(1);
    }
    string line;
    int ligneNumero = 0;
    int itemsCharges = 0;
    while (getline(file, line)) {
        ligneNumero++;
        if (line.empty()) continue;
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ';')) tokens.push_back(token);
        if (tokens.size() != 4) continue;
        if (tokens[0] == "name") continue;
        try {
            string nom = tokens[0];
            int valeur = stoi(tokens[2]);
            int quantite = stoi(tokens[3]);
            Item item(nom, HEAL, valeur);
            player.getInventory().ajouter_item(item, quantite);
            itemsCharges++;
        }
        catch (...) {
            cout << UI::BRIGHT_YELLOW << "Ligne " << ligneNumero << " ignoree." << UI::RESET << endl;
        }
    }
    file.close();
    cout << UI::BRIGHT_GREEN << itemsCharges << " items charges avec succes depuis Items.csv !" << UI::RESET << endl;
}

void Game::loadMonsters() {
    ifstream file("monsters.csv");
    if (!file.is_open()) {
        cout << UI::BRIGHT_RED << "ERREUR : fichier monsters.csv introuvable. Arrêt du jeu." << UI::RESET << endl;
        exit(1);
    }
    string line;
    int ligneNumero = 0;
    int monstresCharges = 0;
    while (getline(file, line)) {
        ligneNumero++;
        if (line.empty() || line[0] == '#') continue;
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ';')) tokens.push_back(token);
        if (tokens.size() < 6) {
            cout << UI::BRIGHT_YELLOW << "Ligne " << ligneNumero << " ignoree (trop peu de colonnes)." << UI::RESET << endl;
            continue;
        }
        try {
            string catStr = tokens[0];
            Category cat;
            if (catStr == "NORMAL") cat = NORMAL;
            else if (catStr == "MINIBOSS") cat = MINIBOSS;
            else if (catStr == "BOSS") cat = BOSS;
            else continue;
            string nom = tokens[1];
            int hp = stoi(tokens[2]);
            int atk = stoi(tokens[3]);
            int def = stoi(tokens[4]);
            int mercyGoal = stoi(tokens[5]);
            vector<string> actIds;
            for (size_t i = 6; i < tokens.size() && i < 10; ++i) {
                if (!tokens[i].empty() && tokens[i] != "-") actIds.push_back(tokens[i]);
            }
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

#pragma region Combat
void Game::displayBattleStatus(Monster* monster) const {
    cout << "\n";
    UI::section("COMBAT CONTRE " + monster->getNom(), UI::BRIGHT_MAGENTA);
    cout << UI::RED << "HP Monstre : " << UI::RESET
        << UI::makeBar(monster->getHp(), monster->getMaxHp(), 10, UI::BRIGHT_RED)
        << " " << monster->getHp() << "/" << monster->getMaxHp() << endl;
    cout << UI::CYAN << "Mercy : " << UI::RESET
        << UI::makeBar(monster->getMercy(), monster->getMercyGoal(), 10, UI::BRIGHT_CYAN)
        << " " << monster->getMercy() << "/" << monster->getMercyGoal() << endl;
    cout << UI::GREEN << "Votre HP : " << UI::RESET
        << UI::makeBar(player.getHp(), player.getMaxHp(), 10, UI::BRIGHT_GREEN)
        << " " << player.getHp() << "/" << player.getMaxHp() << endl << endl;
}

void Game::battle(Monster* monster) {
    cout << UI::BRIGHT_MAGENTA << ">> Un combat commence contre " << monster->getNom() << " !" << UI::RESET << endl;
    while (player.estEnVie() && monster->estEnVie()) {
        displayBattleStatus(monster);
        cout << UI::BRIGHT_YELLOW << "+---------------------------------------------+" << UI::RESET << endl;
        cout << UI::BRIGHT_YELLOW << "| 1. FIGHT 2. ACT 3. ITEM 4. MERCY |" << UI::RESET << endl;
        cout << UI::BRIGHT_YELLOW << "+---------------------------------------------+" << UI::RESET << endl;
        cout << UI::BOLD << "Votre action : " << UI::RESET;
        int action;
        cin >> action;

        if (action == 1) { // FIGHT
            cout << UI::BRIGHT_RED << "Vous attaquez !" << UI::RESET << endl;
            int dmg = rand() % (monster->getMaxHp() + 1);
            if (dmg == 0) {
                cout << UI::YELLOW << "Attaque ratée !" << UI::RESET << endl;
            }
            else {
                monster->subirDegats(dmg);
                cout << UI::RED << "Degats infliges : " << dmg << UI::RESET << endl;
            }
        }
        else if (action == 2) { // ACT
            vector<string> acts = monster->getAvailableActs();
            if (acts.empty()) {
                cout << UI::YELLOW << "Aucune ACT disponible." << UI::RESET << endl;
                continue;
            }
            cout << UI::BRIGHT_CYAN << "Choisissez une ACT :" << UI::RESET << endl;
            for (size_t i = 0; i < acts.size(); ++i) {
                const Act* a = actCatalog.getAct(acts[i]);
                if (a) {
                    cout << UI::CYAN << i + 1 << ". " << a->getTexte() << UI::RESET << endl;
                }
                else {
                    cout << UI::CYAN << i + 1 << ". " << acts[i] << UI::RESET << endl;
                }
            }
            int choix;
            cin >> choix;
            if (choix >= 1 && choix <= static_cast<int>(acts.size())) {
                string id = acts[choix - 1];
                const Act* a = actCatalog.getAct(id);
                if (a) {
                    cout << UI::BRIGHT_CYAN << a->getTexte() << UI::RESET << endl;
                    int mercyRandom = -20 + (rand() % 101);
                    monster->modifier_mercy(mercyRandom);
                }
                else {
                    cout << UI::YELLOW << "ACT non trouvee." << UI::RESET << endl;
                }
            }
        }
        else if (action == 3) { // ITEM
            auto& inv = player.getInventory();
            if (inv.estVide()) {
                cout << UI::YELLOW << "Inventaire vide." << UI::RESET << endl;
                continue;
            }
            inv.afficher_item();
            cout << UI::BOLD << "Numero de l'item a utiliser : " << UI::RESET;
            int idx;
            cin >> idx;
            player.useItem(idx);
        }
        else if (action == 4) { // MERCY
            if (monster->est_epargne()) {
                cout << UI::BRIGHT_GREEN << "Vous epargnez le monstre avec succes !" << UI::RESET << endl;
                player.add_epargne();
                player.add_victoire();
                BestiaryEntry entry(monster->getNom(), monster->getCategory(),
                    monster->getMaxHp(), monster->getAtk(),
                    monster->getDef(), "epargne");
                bestiary.ajouter_monstre(entry);
                return;
            }
            else {
                cout << UI::YELLOW << "Le monstre n'est pas encore pret a etre epargne (Mercy : "
                    << monster->getMercy() << "/" << monster->getMercyGoal() << ")" << UI::RESET << endl;

                // === NOUVEAU : quand MERCY échoue, le monstre attaque avec les nouvelles probas ===
                cout << UI::BRIGHT_MAGENTA << monster->getNom() << " attaque !" << UI::RESET << endl;
                int dmg = 0;
                int r = rand() % 100;
                if (r < 5) {                                      // 5% : le monstre loupe complètement
                    cout << UI::YELLOW << "Le monstre rate son attaque !" << UI::RESET << endl;
                }
                else if (r < 85) {                                // 80% : dégâts divisés par deux
                    dmg = (rand() % 80 + 1) / 2;
                    if (dmg == 0) dmg = 1;
                }
                else {                                            // 15% restant : dégâts normaux
                    dmg = rand() % 80 + 1;
                }
                if (dmg > 0) {
                    player.subirDegats(dmg);
                    cout << UI::BRIGHT_RED << "Degats recus : " << dmg << UI::RESET << endl;
                }
                continue;   // on passe directement au prochain tour du joueur
            }
        }
        else {
            cout << UI::BRIGHT_YELLOW << "Choix invalide." << UI::RESET << endl;
            continue;
        }

        if (!monster->estEnVie()) {
            cout << UI::BRIGHT_GREEN << "Le monstre est vaincu !" << UI::RESET << endl;
            player.add_kill();
            player.add_victoire();
            BestiaryEntry entry(monster->getNom(), monster->getCategory(),
                monster->getMaxHp(), monster->getAtk(),
                monster->getDef(), "tue");
            bestiary.ajouter_monstre(entry);
            return;
        }

        // ====================== TOUR DU MONSTRE (pour les autres actions) ======================
        if (monster->estEnVie()) {
            cout << UI::BRIGHT_MAGENTA << monster->getNom() << " attaque !" << UI::RESET << endl;
            int dmg = 0;
            if (rand() % 100 < 10) {
                dmg = (rand() % 80 + 1);
                if (dmg == 0) dmg = 1;
            }
            else {
                cout << UI::YELLOW << "Le monstre rate son attaque !" << UI::RESET << endl;
            }
            if (dmg > 0) {
                player.subirDegats(dmg);
                cout << UI::BRIGHT_RED << "Degats recus : " << dmg << UI::RESET << endl;
            }
        }

        if (!player.estEnVie()) {
            cout << UI::BRIGHT_RED << "Vous avez ete vaincu ! Defaite immediate." << UI::RESET << endl;
            return;
        }
    }
}

void Game::startRandomBattle() {
    if (monsterPool.empty()) {
        cout << UI::YELLOW << "Aucun monstre disponible pour le combat." << UI::RESET << endl;
        return;
    }
    size_t idx = rand() % monsterPool.size();
    Monster* monster = monsterPool[idx];
    monster->soigner(monster->getMaxHp());
    monster->modifier_mercy(-monster->getMercy());
    cout << "\n";
    cout << UI::BRIGHT_MAGENTA << "=====================================================" << UI::RESET << endl;
    cout << UI::BRIGHT_MAGENTA << " Un " << monster->getNom() << " surgit de la jungle !" << UI::RESET << endl;
    cout << UI::BRIGHT_MAGENTA << "=====================================================" << UI::RESET << endl;
    battle(monster);
}
#pragma endregion

#pragma region Affichages_du_jeu
void Game::showBestiary() const {
    UI::section("BESTIAIRE", UI::BRIGHT_GREEN);
    if (bestiary.estVide()) {
        cout << UI::YELLOW << "Aucun monstre decouvert pour l'instant." << UI::RESET << endl;
        return;
    }
    auto entries = bestiary.getBestiaire();
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        string catStr = (e.getCategory() == NORMAL) ? "NORMAL" :
            (e.getCategory() == MINIBOSS) ? "MINIBOSS" : "BOSS";
        cout << UI::BRIGHT_CYAN << i + 1 << ". " << UI::RESET
            << UI::BOLD << e.getNom() << UI::RESET
            << " " << UI::MAGENTA << "(" << catStr << ")" << UI::RESET << endl;
        cout << " " << UI::RED << "HP max : " << UI::RESET << e.getMaxHp()
            << " | " << UI::YELLOW << "ATK : " << UI::RESET << e.getAtk()
            << " | " << UI::BLUE << "DEF : " << UI::RESET << e.getDef() << endl;
        cout << " " << UI::GREEN << "Resultat : " << UI::RESET << e.getResultat() << endl << endl;
    }
}

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

void Game::showItems() {
    UI::section("INVENTAIRE", UI::BRIGHT_YELLOW);
    auto& inv = player.getInventory();
    if (inv.estVide()) {
        cout << UI::YELLOW << "Votre sac est vide." << UI::RESET << endl;
        return;
    }
    inv.afficher_item();
    cout << "\n" << UI::GREEN << "Tous ces items soignent des HP." << UI::RESET << endl;
    cout << UI::BOLD << "Numero de l'item a utiliser (ou -1 pour retour) : " << UI::RESET;
    int choix;
    cin >> choix;
    if (choix >= 0) player.useItem(choix);
}

void Game::showEnding() const {
    cout << "\n";
    UI::title("FIN DE L'AVENTURE", UI::BRIGHT_MAGENTA);
    int kills = player.getN_kill();
    int spared = player.getN_epargne();
    if (kills == 10) {
        cout << UI::BRIGHT_RED << "FIN GENOCIDAIRE : Vous avez extermine toute la jungle !" << UI::RESET << endl;
    }
    else if (spared == 10) {
        cout << UI::BRIGHT_GREEN << "FIN PACIFISTE : Vous avez apaise la jungle avec ingeniosite !" << UI::RESET << endl;
    }
    else {
        cout << UI::BRIGHT_YELLOW << "FIN NEUTRE : Un equilibre entre force et compassion." << UI::RESET << endl;
    }
    cout << UI::RED << "Tues : " << kills << UI::RESET << endl;
    cout << UI::GREEN << "Epargnes : " << spared << UI::RESET << endl;
    cout << UI::BRIGHT_CYAN << "Merci d'avoir joue à JUNGLER !" << UI::RESET << endl;
}
#pragma endregion

#pragma region Menu_principal
void Game::showMainMenu() {
    cout << "\n";
    UI::section("MENU PRINCIPAL", UI::BRIGHT_GREEN);
    cout << UI::BRIGHT_RED << "1. " << UI::RESET << "Demarrer un combat" << endl;     // ← échangé
    cout << UI::BRIGHT_CYAN << "2. " << UI::RESET << "Bestiaire" << endl;            // ← échangé
    cout << UI::BRIGHT_BLUE << "3. " << UI::RESET << "Statistiques" << endl;
    cout << UI::BRIGHT_YELLOW << "4. " << UI::RESET << "Items" << endl;
    cout << UI::BRIGHT_MAGENTA << "5. " << UI::RESET << "Quitter" << endl;
    cout << UI::BOLD << "Votre choix : " << UI::RESET;
    int choix;
    cin >> choix;
    switch (choix) {
    case 1:                                                                       // ← maintenant combat
        if (!player.estEnVie()) {
            cout << UI::YELLOW << "Vous etes a 0 HP ! Utilisez un item pour vous soigner avant de combattre." << UI::RESET << endl;
        }
        else {
            startRandomBattle();
        }
        break;
    case 2: showBestiary(); break;                                                // ← maintenant bestiaire
    case 3: showStats(); break;
    case 4: showItems(); break;
    case 5:
        cout << UI::BRIGHT_CYAN << "Au revoir, exploratrice !" << UI::RESET << endl;
        exit(0);
    default:
        cout << UI::BRIGHT_YELLOW << "Choix invalide." << UI::RESET << endl;
    }
}
#pragma endregion

#pragma region Lancement_du_jeu
void Game::start() {
    srand(static_cast<unsigned>(time(nullptr)));
    cout << UI::BRIGHT_GREEN << "======================================" << UI::RESET << endl;
    cout << UI::BRIGHT_GREEN << " BIENVENUE DANS JUNGLER " << UI::RESET << endl;
    cout << UI::BRIGHT_GREEN << "======================================" << UI::RESET << endl;
    string nom;
    cout << UI::BOLD << "Nom de votre exploratrice : " << UI::RESET;
    getline(cin, nom);
    if (nom.empty()) nom = "Exploratrice";
    player = Player(nom, 100, 20, 10);
    loadItems();
    loadMonsters();
    loadActs();
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
    while (true) {
        if (player.getN_victoire() >= 10) {
            showEnding();
            break;
        }
        showMainMenu();
    }
}
#pragma endregion
