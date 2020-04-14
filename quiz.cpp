#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <sstream>

using namespace std;

int alllength;
const string fragen = "dat/Fragen.txt";
const string hist = "dat/hist.txt";

class FRAGE
{
public:
    int id;
    string q;
    vector <string> a;
    vector <bool> c;
    string history = "";
    int cor;
    int fal;
    double rate;
    void update()
    {
        cor = 0;
        fal = 0;
        for(int i = 0; i<history.length(); ++i)
        {
            if(history[i]=='1')
                ++cor;
            else
                ++fal;
        }
        rate = (double)cor/(double)history.length();
    }
};

void getcommand(FRAGE * &all);
void randquiz(FRAGE * & all, int len);
void ask(FRAGE & f, FRAGE * & all);
void korrekt(FRAGE & f);
void inkorrekt(FRAGE & f);
void themenquiz(FRAGE * & all);
void schwer(FRAGE * & all);
void thxschwer(FRAGE * & all);
void help();
void ansage();
void schluss(FRAGE * & all);

int main()
{
    FRAGE * all;
    ifstream r;
    r.open(fragen.c_str());
    string ln;
    alllength = 0;
    while(getline(r, ln))
    {
        if(ln[0]=='#')
            ++alllength;
    }
    try
    {
        all = new FRAGE[alllength];
    }
    catch (exception e)
    {
        cout << "Speicherfehler beim Laden der Fragen, bitte um Bestaetigung, das Programm zu schliessen (beliebige nichtleere Eingabe):";
        string a;
        cin >> a;
        exit(1);
    }
    cout << alllength << " Fragen erkannt, werden geladen" << endl;
    r.close();
    r.open(fragen.c_str());
    int current = -1;
    while(getline(r, ln))
    {
        if(ln[0]=='#')
        {
            ++current;
            string ids = "";
            ids += ln[1];
            ids += ln[2];
            ids += ln[3];
            ids += ln[4];
            ln.erase(0,6);
            all[current].id = atoi(ids.c_str());
            all[current].q = ln;
        }
        else if(ln[0]=='-')
        {
            ln.erase(0,1);
            all[current].c.push_back(false);
            all[current].a.push_back(ln);
        }
        else if(ln[0]=='+')
        {
            ln.erase(0,1);
            all[current].c.push_back(true);
            all[current].a.push_back(ln);
        }
    }
    cout << "Laden erfolgreich" << endl;
    cout << "Versuche Antwortsverlauf zu laden" << endl;
    r.close();
    r.open(hist.c_str());
    if(r)
    {
        while(getline(r,ln))
        {
            string ids = "";
            ids += ln[1];
            ids += ln[2];
            ids += ln[3];
            ids += ln[4];
            ln.erase(0,6);
            int idi = atoi(ids.c_str());
            for(int i = 0; i<alllength; ++i)
                if(all[i].id == idi)
                {
                    all[i].history = ln;
                    all[i].update();
                }
        }
        cout << "Laden erfolgreich" << endl;
    }
    else
        cout << "Kein Verlauf vorhanden" << endl;
    r.close();
    //ask(all[2]); //Testen der Fragenfkt.
    while(true)
        getcommand(all);
    return 0;
}

void getcommand(FRAGE * & all)
{
    string comm;
    cout << "> ";
    getline(cin, comm);
    size_t np = string::npos;
    if(comm.find("exit")!=np)
        schluss(all);
    else if(comm.find("quiz")!=np)
        randquiz(all, alllength);
    else if(comm.find("thschwer")!=np)
        thxschwer(all);
    else if(comm.find("thema")!=np)
        themenquiz(all);
    else if(comm.find("schwer")!=np)
        schwer(all);
    else
        help();
}

void randquiz(FRAGE * & all, int len)
{
    ansage();
    int * perm;
    perm = new int[len];
    int zs, m, n;
    srand(time(NULL));
    for(int i = 0; i<len; ++i)
        perm[i]=i;
    for(int i = 0; i<len*2; ++i)
    {
        m=rand()%len;
        n=rand()%len;
        if(m!=n)
        {
            zs=perm[m];
            perm[m]=perm[n];
            perm[n]=zs;
        }
    }
    for(int i = 0; i<len; ++i)
        ask(all[perm[i]], all);
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
}

void ask(FRAGE & f, FRAGE * & all)
{
    cout << endl << "(" << f.id << ") [" << f.cor << "|" << f.fal << "]" << endl;
    cout << f.q << endl << endl;
    stringstream richtig;
    stringstream falsch;
    for(int i = 0; i<f.a.size(); ++i)
    {
        cout << i+1 << ")  " << f.a.at(i) << endl;
        if(f.c.at(i))
            richtig << i+1;
        else
            falsch << i+1;
    }
    cout << endl;
    string antwort;
    getline(cin, antwort);
    if(antwort.find("exit")!=string::npos)
        schluss(all);
    bool corr = true;
    //cout << richtig.str() << "  " << falsch.str() << endl;
    for(int i = 0; i<richtig.str().length(); ++i)
        if(antwort.find(richtig.str()[i])==string::npos)
        {
            corr = false;
            //cout << "didn't find " << richtig.str()[i] << endl;
        }
    for(int i = 0; i<falsch.str().length(); ++i)
        if(antwort.find(falsch.str()[i])!=string::npos)
        {
            corr = false;
            //cout << "found " << falsch.str()[i] << endl;
        }
    if(corr)
        korrekt(f);
    else
        inkorrekt(f);
}

void korrekt(FRAGE & f)
{
    cout << "KORREKTE ANTWORT!" << endl;
    f.history+="1";
}

void inkorrekt(FRAGE & f)
{
    cout << "FALSCHE ANTWORT!" << endl;
    cout << "Richtig(e) Antwort(en):" << endl;
    for(int i = 0; i<f.a.size(); ++i)
    {
        if(f.c.at(i))
            cout << i+1 << ")  " << f.a.at(i) << endl;
    }
    f.history+="0";
}

void schwer(FRAGE * & all)
{
    ansage();
    int len = alllength;
    int * perm;
    perm = new int[len];
    int zs, m, n;
    srand(time(NULL));
    for(int i = 0; i<len; ++i)
        perm[i]=i;
    for(int i = 0; i<len*2; ++i)
    {
        m=rand()%len;
        n=rand()%len;
        if(m!=n)
        {
            zs=perm[m];
            perm[m]=perm[n];
            perm[n]=zs;
        }
    }
    for(int i = 0; i<len; ++i)
        if(all[perm[i]].history.size()>2)
        {
            if(all[perm[i]].rate<=0.8)
                ask(all[perm[i]], all);
        }
        else
            ask(all[perm[i]], all);
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
}

void help()
{
    cout << "Moegliche Kommandos:" << endl;
    cout << "quiz -     Alle Fragen aus allen Themenbereichen in zu-" << endl;
    cout << "           faelliger Reihenfolge" << endl;
    cout << "thema -    Alle Fragen aus einem waehlbaren Themenbereich in" << endl;
    cout << "           zufaelliger Reihenfolge" << endl;
    cout << "schwer -   Nicht zuverlaessig richtig beantwortete Fragen" <<  endl;
    cout << "thschwer - Wie schwer nur zu einem einzelnen Thema" << endl;
    cout << "exit -     Beendet das Programm und speichert den Fortschritt" << endl;
    cout << "           Kann auch als Antwort in einem Quiz gegeben werden," << endl;
    cout << "           um abzubrechen" << endl << endl;
}

void themenquiz(FRAGE * & all)
{
    cout << "Themenbereiche:" << endl;
    cout << "1 - Jagdwaffen, Jagd- und Fanggeraete" << endl;
    cout << "2 - Biologie der WIldarten" << endl;
    cout << "3 - Rechtliche Vorschriften" << endl;
    cout << "4 - Wildhege, Jagdbetrieb und jagdliche Praxis" << endl;
    cout << "5 - Jagdhundewesen" << endl;
    cout << "6 - Naturschutz, Landbau, Forstwesen, Wild- und Jagdschadensverhuetung" << endl;
    cout << "(Bitte Zahl eingeben) ";
    int th;
    string the;
    getline(cin, the);
    th = atoi(the.c_str());
    while(th<1||th>6)
    {
        cout << "ungueltig - Neue Eingabe bitte";
        getline(cin, the);
        th = atoi(the.c_str());
    }
    ansage();
    int len = alllength;
    int * perm;
    perm = new int[len];
    int zs, m, n;
    srand(time(NULL));
    for(int i = 0; i<len; ++i)
        perm[i]=i;
    for(int i = 0; i<len*2; ++i)
    {
        m=rand()%len;
        n=rand()%len;
        if(m!=n)
        {
            zs=perm[m];
            perm[m]=perm[n];
            perm[n]=zs;
        }
    }
    for(int i = 0; i<len; ++i)
        if(all[perm[i]].id <(th+1)*1000&&all[perm[i]].id>=th*1000)
            ask(all[perm[i]], all);
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
}

void thxschwer(FRAGE * & all)
{
    cout << "Themenbereiche:" << endl;
    cout << "1 - Jagdwaffen, Jagd- und Fanggeraete" << endl;
    cout << "2 - Biologie der WIldarten" << endl;
    cout << "3 - Rechtliche Vorschriften" << endl;
    cout << "4 - Wildhege, Jagdbetrieb und jagdliche Praxis" << endl;
    cout << "5 - Jagdhundewesen" << endl;
    cout << "6 - Naturschutz, Landbau, Forstwesen, Wild- und Jagdschadensverhuetung" << endl;
    cout << "(Bitte Zahl eingeben) ";
    int th;
    string the;
    getline(cin, the);
    th = atoi(the.c_str());
    while(th<1||th>6)
    {
        cout << "ungueltig - Neue Eingabe bitte";
        getline(cin, the);
        th = atoi(the.c_str());
    }
    ansage();
    int len = alllength;
    int * perm;
    perm = new int[len];
    int zs, m, n;
    srand(time(NULL));
    for(int i = 0; i<len; ++i)
        perm[i]=i;
    for(int i = 0; i<len*2; ++i)
    {
        m=rand()%len;
        n=rand()%len;
        if(m!=n)
        {
            zs=perm[m];
            perm[m]=perm[n];
            perm[n]=zs;
        }
    }
    for(int i = 0; i<len; ++i)
        if(all[perm[i]].id <(th+1)*1000&&all[perm[i]].id>=th*1000)
        {
            if(all[perm[i]].history.size()>2)
            {
                if(all[perm[i]].rate<=0.8)
                    ask(all[perm[i]], all);
            }
            else
                ask(all[perm[i]], all);
        }
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
}

void ansage()
{
    cout << "Sie haben ein Quiz gestartet" << endl << "Als Antwort zu den Fragen die Nummern der korrekten Optionen eingeben" << endl;
    cout << "Beispiele fuer Antworten im korrekten Format: '123' - '1  2, 3' - '132' usw." << endl;
    cout << "Wichtig ist lediglich, dass alle korrekten Ziffern vorkommen und keine der falschen" << endl;
}

void schluss(FRAGE * & all)
{
    cout << "Programm speichert und schliesst" << endl;
    ofstream o;
    o.open(hist.c_str());
    if(o)
        for(int i = 0; i<alllength; ++i)
        {
            o << "#" << all[i].id << "#" << all[i].history << endl;
        }
    o.close();
    exit(0);
}
