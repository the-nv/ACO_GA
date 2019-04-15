#include<bits/stdc++.h>

using namespace std;

#define POPULATION_SIZE 100
#define SUBJECTS_SIZE 6
#define PROFESSORS_SIZE 3
#define PERIODS 6
#define DAYS 5

const string SUBJECTS="ABCDEF";
const string PROFESSORS="xyz";
const string MAP="yxxzyx";

const int SUB[SUBJECTS_SIZE]={7,3,6,4,5,5};
const int PROF[PROFESSORS_SIZE]={13,12,5};


int random_num(int start, int end) {
    int range=end-start+1;
    return start+(rand()%range);
}

char mutatedGene() {
    int len=SUBJECTS_SIZE;
    int r=random_num(0,len-1);

    return SUBJECTS[r];
}

string createGnome() {
    int len=PERIODS*DAYS;

    string gnome="";

    for(int i=0;i<len;i++) {
        gnome+=mutatedGene();
    }

    return gnome;
}

class Individual {
    public:

    string chromosome;
    int fitness;

    Individual(string chromo) {

        chromosome=chromo;
        fitness=calcFitness();
    }

    Individual mate(Individual parent2) {
        string childChromosome;

        int len=PERIODS*DAYS;

        childChromosome="";

        for(int i=0;i<len;i++) {
            float p=random_num(0,100)/100;

            if(p<0.45)
                childChromosome+=chromosome[i];
            else if(p<0.90)
                childChromosome+=parent2.chromosome[i];
            else
                childChromosome+=mutatedGene();
            
        }

        return Individual(childChromosome);
    }

    int conflictSubHours() {
        int len=PERIODS*DAYS;
        
        int count=0,conflicts=0;

        for(int i=0;i<SUBJECTS_SIZE;i++) {
            count=0;

            char sub=SUBJECTS[i];

            for(int j=0;j<len;j++) {
                if(chromosome[j]==sub)
                    count++;
            }

            conflicts+=abs(count-SUB[i]);
        }

        return conflicts;
    }

    int conflictSubCont() {
        int len=PERIODS*DAYS;

        int conflicts=0;

        for(int i=0;i<len-1;i++) {
            if(chromosome[i]==chromosome[i+1])
                conflicts++;
        }

        return conflicts;
    }

    int conflictProfHours() {
        int len=PERIODS*DAYS;

        int conflicts=0;

        for(int i=0;i<PROFESSORS_SIZE;i++) {
            int count=0;
            char prof=PROFESSORS[i];

            for(int j=0;j<len;j++) {
                if(prof==MAP[chromosome[j]-'A'])
                    count++;
            }

            conflicts+=abs(count-PROF[i]);
        }

        return conflicts;
    }

    int calcFitness() {

        int fitness=0;

        fitness+=conflictSubHours();
        fitness+=conflictSubCont();
        fitness+=conflictProfHours();
        
        return fitness;
    }
};

void display(vector<Individual> population) {
    cout<<"display";

    int len=PERIODS*DAYS;

    for(int i=0;i<len;i++) {
        if(i%PERIODS==0)
            cout<<"\n";
        
        cout<<"\t"<<population[0].chromosome[i];
    }

    cout<<"\n\n\tfitness : "<<population[0].fitness<<endl;
}

bool operator<(const Individual &ind1, const Individual &ind2) {
    return ind1.fitness < ind2.fitness;
}

int main() {
    srand((unsigned)(time(0)));

    vector<Individual> population;
    bool found=false;

    for(int i=0;i<POPULATION_SIZE;i++) {
        string gnome=createGnome();
        population.push_back(Individual(gnome));
    }

    int generation=0,count=0;

    while(!found) {
        sort(population.begin(),population.end());

        if(population[0].fitness<=0 || count==500) {
            found=true;
            break;
        }

        vector <Individual>newGeneration;

        int s=(10*POPULATION_SIZE)/100;

        for(int i=0;i<s;i++) {
            newGeneration.push_back(population[i]);
        }

        s=(90*POPULATION_SIZE)/100;

        for(int i=0;i<s;i++) {
            int r=random_num(0,50);
            Individual parent1=population[r];
            r=random_num(0,50);
            Individual parent2=population[r];

            Individual offspring=parent1.mate(parent2);

            newGeneration.push_back(offspring);
        }

        sort(newGeneration.begin(), newGeneration.end());

        if(newGeneration[0].fitness==population[0].fitness)
            count++;
        else
            count=0;
        

        population=newGeneration;

        // display(population);
        generation++;
    }

    display(population);
}