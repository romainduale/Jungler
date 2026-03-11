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

using namespace std;

Game::Game() : player("Inconnu", 50, 10, 5) {
    // Constructeur par défaut (sera écrasé dans start())
}

Game::~Game() {
    for (auto m : monsterPool) {
        delete m;
    }
}

void Game::loadActs() {
    // Catalogue ACT (minimum 8 actions, 3 négatives, thème jungle)
    actCatalog.addAct(Act("COMPLIMENT", "Vous complimentez la fourrure soyeuse du monstre.", 25));
    actCatalog.addAct(Act("DANSE_TRIBALE", "Vous dansez comme un singe pour amuser le monstre.", 30));
    actCatalog.addAct(Act("OFFRIR_FRUIT", "Vous offrez une mangue juteuse au monstre.", 40));
    actCatalog.addAct(Act("CHATOUILLER", "Vous chatouillez le monstre avec une feuille géante.", 15));
    actCatalog.addAct(Act("INSULTER", "Vous insultez son odeur de jungle humide.", -20));
    actCatalog.addAct(Act("MENACER", "Vous menacez de brûler les lianes autour de lui.", -10));
    actCatalog.addAct(Act("CHANTER", "Vous chantez une mélodie apaisante de la jungle.", 20));
    actCatalog.addAct(Act("RACONTER", "Vous racontez une légende absurde sur les totems mayas.", 35));
    actCatalog.addAct(Act("CRIER", "Vous imitez un cri de jaguar enragé.", -15));
    actCatalog.addAct(Act("CARESSER", "Vous caressez gentiment le monstre pour le calmer.", 10));
}

void Game::loadItems() {
    ifstream file("items.csv");
    if (!file.is_open()) {
        cout << "ERREUR : fichier items.csv introuvable. Arrêt du jeu." << endl;
        exit(1);
    }

    string line;
    getline(file, line); // saute l'en-tête

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ';')) {
            tokens.push_back(token);
        }
        if (tokens.size() == 4) {
            string nom = tokens[0];
            string typeStr = tokens[1];
            int valeur = stoi(tokens[2]);
            int quantite = stoi(tokens[3]);

            ItemType typ = HEAL; // seul type supporté
            Item item(nom, typ, valeur);
            player.getInventory().ajouter_item(item, quantite);
        }
    }
    file.close();
}

void Game::loadMonsters() {
    ifstream file("monsters.csv");
    if (!file.is_open()) {
        cout << "ERREUR : fichier monsters.csv introuvable. Arrêt du jeu." << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ';')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 6) continue; // ligne mal formée → ignorée

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
            if (tokens[i] != "-" && !tokens[i].empty()) {
                actIds.push_back(tokens[i]);
            }
        }

        Monster* m = new Monster(nom, hp, atk, def, cat, mercyGoal, actIds);
        monsterPool.push_back(m);
    }
    file.close();

    if (monsterPool.empty()) {
        cout << "Aucun monstre chargé depuis monsters.csv." << endl;
    }
}

void Game::displayBattleStatus(Monster* monster) const {
    cout << "\n=== " << monster->getNom() << " ===" << endl;

    // Barre HP monstre
    cout << "HP     : [";
    int hpBars = (monster->getHp() * 10) / monster->getMaxHp();
    for (int i = 0; i < 10; ++i) {
        cout << (i < hpBars ? "#" : "-");
    }
    cout << "] " << monster->getHp() << "/" << monster->getMaxHp() << endl;

    // Barre Mercy monstre
    cout << "Mercy  : [";
    int mercyBars = (monster->getMercy() * 10) / monster->getMercyGoal();
    for (int i = 0; i < 10; ++i) {
        cout << (i < mercyBars ? "#" : "-");
    }
    cout << "] " << monster->getMercy() << "/" << monster->getMercyGoal() << endl;

    cout << "Votre HP : " << player.getHp() << "/" << player.getMaxHp() << endl << endl;
}

void Game::battle(Monster* monster) {
    cout << "Un combat commence contre " << monster->getNom() << " !" << endl;

    while (player.estEnVie() && monster->estEnVie()) {
        displayBattleStatus(monster);

        cout << "1. FIGHT    2. ACT    3. ITEM    4. MERCY" << endl;
        cout << "Votre action : ";
        int action;
        cin >> action;

        if (action == 1) { // FIGHT
            cout << "Vous attaquez !" << endl;
            int dmg = rand() % (monster->getMaxHp() + 1);
            if (dmg == 0) {
                cout << "Attaque ratée !" << endl;
            }
            else {
                monster->subirDegats(dmg);
                cout << "Dégâts infligés : " << dmg << endl;
            }
        }
        else if (action == 2) { // ACT
            vector<string> acts = monster->getAvailableActs();
            if (acts.empty()) {
                cout << "Aucune ACT disponible." << endl;
                continue;
            }
            cout << "Choisissez une ACT :" << endl;
            for (size_t i = 0; i < acts.size(); ++i) {
                const Act* a = actCatalog.getAct(acts[i]);
                if (a) {
                    cout << i + 1 << ". " << a->getTexte()
                        << " (impact : " << a->getMercy_impact() << ")" << endl;
                }
                else {
                    cout << i + 1 << ". " << acts[i] << " (ACT inconnue)" << endl;
                }
            }
            int choix;
            cin >> choix;
            if (choix >= 1 && choix <= static_cast<int>(acts.size())) {
                string id = acts[choix - 1];
                const Act* a = actCatalog.getAct(id);
                if (a) {
                    cout << a->getTexte() << endl;
                    monster->modifier_mercy(a->getMercy_impact());
                }
                else {
                    cout << "ACT non trouvée." << endl;
                }
            }
        }
        else if (action == 3) { // ITEM
            auto& inv = player.getInventory();
            if (inv.estVide()) {
                cout << "Inventaire vide." << endl;
                continue;
            }
            inv.afficher_item();
            cout << "Numéro de l'item à utiliser : ";
            int idx;
            cin >> idx;
            player.useItem(idx);
        }
        else if (action == 4) { // MERCY
            if (monster->est_epargne()) {
                cout << "Vous épargnez le monstre avec succès !" << endl;
                player.add_epargne();
                player.add_victoire();

                BestiaryEntry entry(monster->getNom(), monster->getCategory(),
                    monster->getMaxHp(), monster->getAtk(),
                    monster->getDef(), "épargné");
                bestiary.ajouter_monstre(entry);
                return;
            }
            else {
                cout << "Le monstre n'est pas encore prêt à être épargné (Mercy : "
                    << monster->getMercy() << "/" << monster->getMercyGoal() << ")" << endl;
            }
        }
        else {
            cout << "Choix invalide." << endl;
            continue;
        }

        // Vérification après action du joueur
        if (!monster->estEnVie()) {
            cout << "Le monstre est vaincu !" << endl;
            player.add_kill();
            player.add_victoire();

            BestiaryEntry entry(monster->getNom(), monster->getCategory(),
                monster->getMaxHp(), monster->getAtk(),
                monster->getDef(), "tué");
            bestiary.ajouter_monstre(entry);
            return;
        }

        // Tour du monstre
        if (monster->estEnVie()) {
            cout << monster->getNom() << " attaque !" << endl;
            int dmg = rand() % (player.getMaxHp() + 1);
            if (dmg == 0) {
                cout << "Attaque du monstre ratée !" << endl;
            }
            else {
                player.subirDegats(dmg);
                cout << "Dégâts reçus : " << dmg << endl;
            }
        }

        if (!player.estEnVie()) {
            cout << "Vous avez été vaincu ! Défaite immédiate." << endl;
            return;
        }
    }
}

void Game::startRandomBattle() {
    if (monsterPool.empty()) {
        cout << "Aucun monstre disponible pour le combat." << endl;
        return;
    }

    size_t idx = rand() % monsterPool.size();
    Monster* monster = monsterPool[idx];

    // Reset complet du monstre pour le nouveau combat
    monster->soigner(monster->getMaxHp());
    monster->modifier_mercy(-monster->getMercy());

    cout << "\n=== Un " << monster->getNom() << " surgit de la jungle ! ===" << endl;
    battle(monster);
}

void Game::showBestiary() const {
    cout << "\n=== BESTIAIRE ===" << endl;
    if (bestiary.estVide()) {
        cout << "Aucun monstre découvert pour l'instant." << endl;
        return;
    }

    auto entries = bestiary.getBestiaire();
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        string catStr = (e.getCategory() == NORMAL) ? "NORMAL" :
            (e.getCategory() == MINIBOSS) ? "MINIBOSS" : "BOSS";

        cout << i + 1 << ". " << e.getNom() << " (" << catStr << ")" << endl;
        cout << "   HP max : " << e.getMaxHp() << " | ATK : " << e.getAtk()
            << " | DEF : " << e.getDef() << endl;
        cout << "   Résultat : " << e.getResultat() << endl << endl;
    }
}

void Game::showStats() const {
    cout << "\n=== STATISTIQUES ===" << endl;
    cout << "Nom          : " << player.getNom() << endl;
    cout << "HP           : " << player.getHp() << " / " << player.getMaxHp() << endl;
    cout << "Attaque      : " << player.getAtk() << endl;
    cout << "Défense      : " << player.getDef() << endl;
    cout << "Victoires    : " << player.getN_victoire() << " / 10" << endl;
    cout << "Tués         : " << player.getN_kill() << endl;
    cout << "Épargnés     : " << player.getN_epargne() << endl;
}

void Game::showItems() {
    cout << "\n=== INVENTAIRE ===" << endl;
    auto& inv = player.getInventory();
    if (inv.estVide()) {
        cout << "Votre sac est vide." << endl;
        return;
    }

    inv.afficher_item();
    cout << "\nTous ces items soignent des HP." << endl;
    cout << "Numéro de l'item à utiliser (ou -1 pour retour) : ";

    int choix;
    cin >> choix;
    if (choix >= 0) {
        player.useItem(choix);
    }
}

void Game::showEnding() const {
    cout << "\n=====================================================" << endl;
    cout << "                 FIN DE L'AVENTURE                   " << endl;
    cout << "=====================================================" << endl;

    int kills = player.getN_kill();
    int spared = player.getN_epargne();

    if (kills == 10) {
        cout << "FIN GÉNOCIDAIRE : Vous avez exterminé toute la jungle !" << endl;
    }
    else if (spared == 10) {
        cout << "FIN PACIFISTE : Vous avez apaisé la jungle avec ingéniosité !" << endl;
    }
    else {
        cout << "FIN NEUTRE : Un équilibre entre force et compassion." << endl;
    }

    cout << "Tués     : " << kills << endl;
    cout << "Épargnés : " << spared << endl;
    cout << "Merci d'avoir joué à JUNGLER !" << endl;
}

void Game::showMainMenu() {
    cout << "\n=== MENU PRINCIPAL ===" << endl;
    cout << "1. Bestiaire" << endl;
    cout << "2. Démarrer un combat" << endl;
    cout << "3. Statistiques" << endl;
    cout << "4. Items" << endl;
    cout << "5. Quitter" << endl;
    cout << "Votre choix : ";

    int choix;
    cin >> choix;

    switch (choix) {
    case 1:
        showBestiary();
        break;
    case 2:
        if (!player.estEnVie()) {
            cout << "Vous êtes à 0 HP ! Utilisez un item pour vous soigner avant de combattre." << endl;
        }
        else {
            startRandomBattle();
        }
        break;
    case 3:
        showStats();
        break;
    case 4:
        showItems();
        break;
    case 5:
        cout << "Au revoir, exploratrice !" << endl;
        exit(0);
    default:
        cout << "Choix invalide." << endl;
    }
}

void Game::start() {
    srand(static_cast<unsigned>(time(nullptr)));

    cout << "======================================" << endl;
    cout << "          BIENVENUE DANS JUNGLER       " << endl;
    cout << "======================================" << endl;

    string nom;
    cout << "Nom de votre exploratrice : ";
    getline(cin, nom);
    if (nom.empty()) nom = "Exploratrice";

    player = Player(nom, 100, 20, 10);

    loadItems();
    loadMonsters();
    loadActs();

    // Résumé initial
    cout << "\n--- Résumé de l'expédition ---" << endl;
    cout << "Nom : " << player.getNom() << endl;
    cout << "HP  : " << player.getHp() << "/" << player.getMaxHp() << endl;
    cout << "Inventaire initial :" << endl;
    if (player.getInventory().estVide()) {
        cout << "  (vide)" << endl;
    }
    else {
        player.getInventory().afficher_item();
    }
    cout << "\nL'aventure commence...\n";

    // Boucle principale du jeu
    while (true) {
        if (player.getN_victoire() >= 10) {
            showEnding();
            break;
        }
        showMainMenu();
    }
}