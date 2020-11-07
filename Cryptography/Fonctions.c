#include "conio21/conio2.h"
#include "Fonctions.h"

int encrypter(char* cle, char* fichier, char* fichier_enc)
{
    FILE* f = fopen(fichier, "r");
    if (f == NULL)
    {
        gotoxy(31, wherey()+2);
        textcolor(LIGHTRED);
        fprintf(stderr, "Erreur a l'ouverture du fichier");
        return 1;
    }

    FILE* g = fopen(fichier_enc, "w+");

    if (g == NULL)
    {
        gotoxy(31, wherey()+2);
        textcolor(LIGHTRED);
        fprintf(stderr, "Erreur a la creation du fichier");
        return 2;
    }

    int i = 0, j, taille_cle = strlen(cle);
    unsigned char params[3], ind, resultat, c = fgetc(f), secret = (xor_cle(cle) + taille_cle);

    while (!feof(f))
    {
        j = i % taille_cle;
        ind = ((cle[j]^secret) + i) % 3; //ind = (cle[j] % 3); //ind = (cle[j] % 3)^(msg[i] % 3);
        params[ind] = ((c & 0x30) / 0x10); //ou bien b = (msg[0] & 0x30) >> 4;
        params[(ind + 1) % 3] = ((c & 0x0C) / 0x04); //ou bien c = (msg[0] & 0x0C) >> 2;
        params[(ind + 2) % 3] = c & 0x03;

        params[0] ^= ind; // 1ere operation: le XOR
        params[1] = (~params[1])&3; // 2eme operation: le complement à 1
        if ((((ind + 2) % 3) % 2) == 0) // 3eme operation: decalage circulaire
        {
            params[2] = ((params[2] << 1) & 3) + (params[2] & 0x02) / 0x02; // decalage circulaire à gauche
        }
        else
        {
            params[2] = ((params[2] >> 1) & 3) + (params[2] & 0x01) * 0x02; // decalage circulaire à droite
        }

        resultat = (c & 0xC0) + params[0]*0x10 + params[1]*0x04 + params[2];
        resultat = ~resultat;
        resultat ^= cle[j];

        // Traitement des cas spéciaux
        if ((resultat < 0x20) || (resultat == 0x7F) || (resultat == 0xFF) || (resultat == '#'))
        {
            fputc('#', g);
            resultat += (i+j) % 95 + 32;
        }

        fputc(resultat, g);
        i++;
        c = fgetc(f);
    }

    fclose(g);
    fclose(f);
    return 0;
}

int decrypter(char* cle, char* fichier, char* fichier_dec)
{
    FILE* f = fopen(fichier, "r");
    if (f == NULL)
    {
        gotoxy(31, wherey()+2);
        textcolor(LIGHTRED);
        fprintf(stderr, "Erreur a l'ouverture du fichier");
        return 1;
    }

    FILE* g = fopen(fichier_dec, "w+");

    if (g == NULL)
    {
        gotoxy(31, wherey()+2);
        textcolor(LIGHTRED);
        fprintf(stderr, "Erreur a la creation du fichier");
        return 2;
    }

    int i = 0, j, taille_cle = strlen(cle);
    unsigned char c, special = 0, secret = (xor_cle(cle) + taille_cle);
    fseek(f, 0, SEEK_SET);

    while ((c = fgetc(f)) != 0xFF)
    {
        j = i % taille_cle;
        if (special == 0)
        {
            if (c == '#') // on est tombé sur un cas spécial
            {
                special = 1; //activer l'indicateur special
                continue; // on saute le #
            }
            else
            {
                /**decryptage noraml */
                decryptage_noraml(&c, cle[j], secret, i, j);
                i++;
            }
        }
        else
        {
            /**decryptage special */
            c = c - (32 + (i+j)%95); // on retranche la valeur ajoutée
            decryptage_noraml(&c, cle[j], secret, i, j); // on decrypte alors
            special = 0; //desactiver l'indicateur special
            i++;
        }
        fputc(c, g);
    }

    fclose(g);
    fclose(f);
    return 0;
}

void decryptage_noraml(unsigned char* c, char cle, unsigned char secret, int i, int j)
{
    unsigned char params[3], ind;

    *c ^= cle;
    *c = ~(*c);

    ind = ((cle^secret) + i) % 3;
    params[0] = (((*c) & 0x30) / 0x10) ^ ind;
    params[1] = (~(((*c) & 0x0C) / 0x04))&3;
    params[2] = (*c) & 0x03;

    if ((((ind + 2) % 3) % 2) == 0) // 3eme operation: decalage circulaire
    {
        params[2] = ((params[2] >> 1) & 3) + (params[2] & 0x01) * 0x02; // decalage circulaire à droite
    }
    else
    {
        params[2] = ((params[2] << 1) & 3) + (params[2] & 0x02) / 0x02; // decalage circulaire à gauche
    }

    *c = (*c & 0xC0) + params[ind]*0x10 + params[(ind+1)%3]*0x04 + params[(ind+2)%3];
}

int xor_cle(char* cle)
{
    int n = strlen(cle), i, s = 0;
    for(i=0; i<n; i++)
    {
        s ^= cle[i];
    }
    return s;
}

int comparer(char* fic1, char* fic2)
{
    FILE* f = fopen(fic1, "r");
    if (f == NULL)
    {
        gotoxy(31, wherey()+2);
        textcolor(LIGHTRED);
        fprintf(stderr, "Erreur a l'ouverture du premier fichier");
        return 1;
    }

    FILE* g = fopen(fic2, "r");

    if (g == NULL)
    {
        gotoxy(31, wherey()+2);
        textcolor(LIGHTRED);
        fprintf(stderr, "Erreur a l'ouverture du deuxieme fichier");
        return 2;
    }

    char c1, c2;
    while(1)
    {
        c1 = fgetc(f);
        c2 = fgetc(g);

        if (c1 != c2)
        {
            fclose(g);
            fclose(f);
            gotoxy(31, wherey()+2);
            textcolor(LIGHTRED);
            fprintf(stderr, "Les deux fichiers ne sont pas identiques");
            return 3;
        }


        if (feof(f) && feof(g))
        {
            fclose(g);
            fclose(f);
            gotoxy(31, wherey()+2);
            textcolor(LIGHTGREEN);
            fprintf(stderr, "Les deux fichiers sont identiques");
            return 0;
        }
        else if ((feof(f) && !feof(g)) || (!feof(f) && feof(g)))
        {
            fclose(g);
            fclose(f);
            gotoxy(31, wherey()+2);
            textcolor(LIGHTRED);
            fprintf(stderr, "Les deux fichiers ne sont pas identiques");
            return 4;
        }
    }
}

void viderBuffer(void)
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getch();
    }
}

int lire(char *chaine, int longueur)
{
    char *positionEntree = NULL;

    if (fgets(chaine, longueur, stdin) != NULL)
    {
        positionEntree = strchr(chaine, '\n');
        if (positionEntree != NULL)
        {
            *positionEntree = '\0';
        }
        else
        {
            viderBuffer();
        }
        return 1;
    }
    else
    {
        viderBuffer();
        return 0;
    }
}

void bienvenue(void)
{
    textcolor(LIGHTBLUE);
    cputsxy(38, 10, "Bienvenue  au  programme  d'encryptage");
    textcolor(LIGHTRED);
    cputsxy(50, 13, "Realisé par:");
    textcolor(LIGHTGREEN);
    cputsxy(39, 16, "Abdeslam BENAMEUR && Amel BELDJILALI");
    textcolor(LIGHTMAGENTA);
    cputsxy(37, 21, "Appuyez sur une touche pour continuer ... ");
    getch();
}

void menu(void)
{
    char choix[2], cle[20], file1[300], file2[10]="enc00.txt", file3[10]="dec00.txt", cmp1[300], cmp2[300];
    int nb_dec = 0, nb_enc = 0, continuer = 1;
    while (continuer)
    {
        clrscr();
        textcolor(CYAN);
        cputsxy(32, 5, "*** M E N U ***");
        textcolor(LIGHTCYAN);
        cputsxy(31, 7, "1-Encrypter un fichier");
        cputsxy(31, 9, "2-Décrypter un fichier");
        cputsxy(31, 11, "3-Comparer source et résultat");
        cputsxy(31, 13, "4-Quitter");
        textcolor(CYAN);
        cputsxy(35, 15, "Faites un choix: ");
        choix[0] = getch();
        choix[1] = '\0';
        textcolor(LIGHTCYAN);
        printf("%s ", choix);
        switch (choix[0])
        {
            case '1': // encryptage
                textcolor(YELLOW);
                cputsxy(31, 17, "Le fichier à encrypter ou bien juste tapez fic1.txt ? ");
                lire(file1, 300);
                cputsxy(31, 19, "Saisissez la clé d'encryptage ");
                lire(cle, 20);
                file2[3] = (nb_enc / 10) % 10 + '0';
                file2[4] = nb_enc % 10 + '0';
                if (!encrypter(cle, file1, file2))
                {
                    nb_enc += 1;
                    textcolor(LIGHTGREEN);
                    cputsxy(31, 21, "L'encryptage est effectué avec succès");
                    gotoxy(31, 23);
                    printf("Le fichier %s ", file2);
                    cputsxy(wherex(), wherey(), "est généré");
                }
                cputsxy(35, 25, "Appuyez sur une touche pour continuer ... ");
                getch();
                break;
            case '2': //decryptage
                if (nb_enc != 0) // on a au moins un encryptage
                {
                    textcolor(YELLOW);
                    cputsxy(31, 17, "Le fichier à décrypter ? ");
                    lire(file1, 300);
                    cputsxy(31, 19, "Saisissez la clé d'encryptage ");
                    lire(cle, 20);
                    file3[3] = ((nb_dec / 10) % 10) + '0';
                    file3[4] = (nb_dec % 10) + '0';
                    if (!decrypter(cle, file2, file3))
                    {
                        nb_dec += 1;
                        textcolor(LIGHTGREEN);
                        cputsxy(31, 21, "Le décryptage est effectué avec succès");
                        gotoxy(31, 23);
                        printf("Le fichier %s ", file3);
                        cputsxy(wherex(), wherey(), "est généré");
                    }
                    cputsxy(35, 25, "Appuyez sur une touche pour continuer ... ");
                    getch();
                }
                else
                {
                    textcolor(LIGHTRED);
                    cputsxy(31, 20, "Vous devez d'abord réaliser au moins un encryptage ");
                    cputsxy(35, 25, "Appuyez sur une touche pour continuer ... ");
                    getch();
                }
                break;
            case '3': // la comparaion
                if (nb_dec != 0) // on a au moins un decryptage
                {
                    textcolor(YELLOW);
                    cputsxy(31, 17, "Le 1er fichier ? ");
                    lire(cmp1, 300);
                    cputsxy(31, 19, "Le 2ème fichier ? ");
                    lire(cmp2, 300);
                    comparer(cmp1, cmp2);
                    cputsxy(35, 25, "Appuyez sur une touche pour continuer ... ");
                    getch();
                }
                else
                {
                    textcolor(LIGHTRED);
                    cputsxy(31, 20, "Vous devez d'abord réaliser au moins un encryptage et un décryptage");
                    cputsxy(35, 25, "Appuyez sur une touche pour continuer ... ");
                    getch();
                }
                break;
            case '4': // quitter
                continuer = 0;
                textcolor(LIGHTMAGENTA);
                cputsxy(30, 22, "Merci d'avoir utilisé notre programme ");
                getch();
                break;
        }
    }
}
