#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <cstdlib>

using namespace std;

int alllength;
const string fragen = "dat/Fragen.txt";
const string hist = "dat/hist.txt";
const string statu = "dat/status.txt";

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

class STATUS
{
public:
    FRAGE * all;
    int * perm;
    int aktuell;
    int quizid;
};

STATUS * speicher;

void exiting();
void getcommand(STATUS & quo);
bool readstat(STATUS & quo);
void writestat(STATUS & quo);
void randquiz(STATUS & quo);
void ask(FRAGE & f, STATUS & quo);
void korrekt(FRAGE & f);
void inkorrekt(FRAGE & f);
void permutiere(STATUS & quo);
void themenquiz(STATUS & quo);
void schwer(STATUS & quo);
void thxschwer(STATUS & quo);
void frag(STATUS & quo);
void help();
void ansage();
void schluss(STATUS & quo);

int main()
{
    atexit(exiting);
    STATUS quo;
    speicher = &quo;
    quo.aktuell=0;
    quo.quizid=0;
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
        quo.all = new FRAGE[alllength];
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
            quo.all[current].id = atoi(ids.c_str());
            quo.all[current].q = ln;
        }
        else if(ln[0]=='-')
        {
            ln.erase(0,1);
            quo.all[current].c.push_back(false);
            quo.all[current].a.push_back(ln);
        }
        else if(ln[0]=='+')
        {
            ln.erase(0,1);
            quo.all[current].c.push_back(true);
            quo.all[current].a.push_back(ln);
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
                if(quo.all[i].id == idi)
                {
                    quo.all[i].history = ln;
                    quo.all[i].update();
                }
        }
        cout << "Laden erfolgreich" << endl;
    }
    else
        cout << "Kein Verlauf vorhanden" << endl;
    r.close();

    cout << "Versuche Zwischenspeicher zu laden" << endl;
    if(readstat(quo))
    {
        cout << "Laden erfolgreich" << endl;
        if(quo.aktuell!=0)
        {
            cout << "Es sieht aus, als waere das Quiz der letzten" << endl;
            cout << "Sitzung noch nicht zuende" << endl;
            cout << "Genau dort weitermachen? (j/n) " << endl;
            string in;
            getline(cin, in);
            if(in.find("j")!=string::npos)
            {
                if(quo.quizid>20)
                    thxschwer(quo);
                else if(quo.quizid==20)
                    schwer(quo);
                else if(quo.quizid>10)
                    themenquiz(quo);
                else if(quo.quizid==10)
                    randquiz(quo);
            }
            else
            {
                quo.quizid = 0;
                quo.aktuell = 0;
                //cout << "aktuell - " << quo.aktuell << endl;
            }
        }
    }
    else
    {
        cout << "Kein Zwischenspeicher" << endl;
    }
    while(true)
        getcommand(quo);
    return 0;
}

void exiting()
{
    schluss(*speicher);
}

void getcommand(STATUS & quo)
{
    string comm;
    cout << "> ";
    getline(cin, comm);
    size_t np = string::npos;
    if(comm.find("exit")!=np)
        schluss(quo);
    else if(comm.find("frage")!=np)
    {
        frag(quo);
    }
    else if(comm.find("quiz")!=np)
    {
        permutiere(quo);
        randquiz(quo);
    }
    else if(comm.find("thschwer")!=np)
    {
        permutiere(quo);
        thxschwer(quo);
    }
    else if(comm.find("thema")!=np)
    {
        permutiere(quo);
        themenquiz(quo);
    }
    else if(comm.find("schwer")!=np)
    {
        permutiere(quo);
        schwer(quo);
    }
    else
        help();
}

void permutiere(STATUS & quo)
{
    int len = alllength;
    quo.perm = new int[len];
    int zs, m, n;
    srand(time(NULL));
    for(int i = 0; i<len; ++i)
        quo.perm[i]=i;
    for(int i = 0; i<len*2; ++i)
    {
        m=rand()%len;
        n=rand()%len;
        if(m!=n)
        {
            zs=quo.perm[m];
            quo.perm[m]=quo.perm[n];
            quo.perm[n]=zs;
        }
    }
}

void randquiz(STATUS & quo)
{
    quo.quizid=10;
    int len = alllength;
    ansage();
    for(; quo.aktuell<len; ++quo.aktuell)
        ask(quo.all[quo.perm[quo.aktuell]], quo);
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
    quo.quizid=0;
    quo.aktuell=0;
}

void writestat(STATUS & quo)
{
    ofstream w;
    w.open(statu.c_str());
    w << "#" << quo.quizid << "#" << "quizid#" << endl;
    w << "#" << quo.aktuell << "#" << "aktuell#" << endl;
    w << "#perm#" << endl;
    for(int i = 0; i<alllength; ++i)
        w << quo.perm[i] << endl;
    w << "#permend#" << endl;
}

bool readstat(STATUS & quo)
{
    ifstream r;
    r.open(statu.c_str());
    string ln;
    int a = 0;
    quo.perm = new int[alllength];
    while(getline(r,ln))
    {
        if(ln=="#perm#"&&a==2)
        {
            //cout << "lese perm" << endl;
            int i = 0;
            while(getline(r,ln)&&i<alllength)
            {
                quo.perm[i]=atoi(ln.c_str());
                ++i;
                //cout << atoi(ln.c_str()) << endl;
            }
            if(ln=="#permend#")
            {
                //cout << "perm gelesen" << endl;
                r.close();
                //cout << quo.aktuell << " (aktuell); " << quo.quizid << " (quizid);" << endl;
                return true;
            }
        }
        else if(ln[0]=='#')
        {
            string ins = "";
            for(int i = 1; i<ln.size()&&ln[i]!='#'; ++i)
                ins += ln[i];
            int in = atoi(ins.c_str());
            if(a==0)
            {
                quo.quizid = in; /*cout << "quizid gelesen " << in << endl;*/
            }
            else if(a==1)
            {
                quo.aktuell = in; /*cout << "aktuell gelesen " << in << endl;*/
            }
            ++a;
        }
    }
    quo.quizid = 0;
    quo.aktuell = 0;
    r.close();
    return false;
}

void ask(FRAGE & f, STATUS & quo)
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
        schluss(quo);
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

void schwer(STATUS & quo)
{
    quo.quizid=20;
    ansage();
    int len = alllength;
    for(; quo.aktuell<len; ++quo.aktuell)
        if(quo.all[quo.perm[quo.aktuell]].history.size()>2)
        {
            if(quo.all[quo.perm[quo.aktuell]].rate<=0.8)
                ask(quo.all[quo.perm[quo.aktuell]], quo);
        }
        else
            ask(quo.all[quo.perm[quo.aktuell]], quo);
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
    quo.quizid=0;
    quo.aktuell=0;
}

void help()
{
    cout << "Moegliche Kommandos:" << endl;
    cout << "frage -    Eine einzelne Frage, ID wird abgefragt" << endl;
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

void themenquiz(STATUS & quo)
{
    if(quo.quizid==0)
    {
        cout << "Themenbereiche:" << endl;
        cout << "1 - Jagdwaffen, Jagd- und Fanggeraete" << endl;
        cout << "2 - Biologie der Wildarten" << endl;
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
        quo.quizid=10+th;
    }
    int th = quo.quizid-10;
    ansage();
    int len = alllength;
    for(; quo.aktuell<len; ++quo.aktuell)
        if(quo.all[quo.perm[quo.aktuell]].id <(th+1)*1000&&quo.all[quo.perm[quo.aktuell]].id>=th*1000)
            ask(quo.all[quo.perm[quo.aktuell]], quo);
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
    quo.aktuell=0;
    quo.quizid=0;
}

void thxschwer(STATUS & quo)
{
    if(quo.quizid==0)
    {
        cout << "Themenbereiche:" << endl;
        cout << "1 - Jagdwaffen, Jagd- und Fanggeraete" << endl;
        cout << "2 - Biologie der Wildarten" << endl;
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
        quo.quizid=20+th;
    }
    int th = quo.quizid-20;
    ansage();
    int len = alllength;
    for(; quo.aktuell<len; ++quo.aktuell)
        if(quo.all[quo.perm[quo.aktuell]].id <(th+1)*1000&&quo.all[quo.perm[quo.aktuell]].id>=th*1000)
        {
            if(quo.all[quo.perm[quo.aktuell]].history.size()>2)
            {
                if(quo.all[quo.perm[quo.aktuell]].rate<=0.8)
                    ask(quo.all[quo.perm[quo.aktuell]], quo);
            }
            else
                ask(quo.all[quo.perm[quo.aktuell]], quo);
        }
    cout << "Quiz abgeschlossen - 'exit' zum Speichern und Schliessen" << endl;
    quo.quizid=0;
    quo.aktuell=0;
}

void frag(STATUS & quo)
{
    cout << "Welche Frage?" << endl;
    string ine;
    getline(cin, ine);
    int in = atoi(ine.c_str());
    bool gefragt = false;
    for(int i = 0; i<alllength; ++i)
        if(quo.all[i].id == in)
        {
            cout << i << endl;
            ask(quo.all[i], quo);
            gefragt = true;
        }
    if(!gefragt)
        cout << "ID nicht gefunden" << endl;
}

void ansage()
{
    cout << "Sie haben ein Quiz gestartet" << endl << "Als Antwort zu den Fragen die Nummern der korrekten Optionen eingeben" << endl;
    cout << "Beispiele fuer Antworten im korrekten Format: '123' - '1  2, 3' - '132' usw." << endl;
    cout << "Wichtig ist lediglich, dass alle korrekten Ziffern vorkommen und keine der falschen" << endl;
}

void schluss(STATUS & quo)
{
    cout << "Programm speichert und schliesst" << endl;
    ofstream o;
    o.open(hist.c_str());
    if(o)
        for(int i = 0; i<alllength; ++i)
        {
            o << "#" << quo.all[i].id << "#" << quo.all[i].history << endl;
        }
    o.close();
    writestat(quo);
    exit(0);
}
