#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <fstream>


#define numCities 30
#define numAnts 5
#define maxTime 1000

//int numCities;
//int numAnts;

using namespace std;

const string SUBJECTS="ABCDEF";
const int SUB[]={7,3,6,4,5,5};
const string PROFESSORS="xyz";
const int PROF[]={14,12,4};
const string MAP="xxyzyx";

int alpha = 3;                  // influence of pheromone in direction
int beta = 2;                   // influence of adjacent node distance
double rho = 0.01;              // pheromone decrease factor
double Q = 2.0;                 // pheromone increase factor 

int assign(int i, int j, int temp[]) {
	int d=0;

	int k,l;

	for(k=0;k<6;k++)
		if(i<temp[k])
			break;

	for(l=0;l<6;l++)
		if(j<temp[l])
			break;

	if(l==k)
		d=100;
	else
		d=1;

	if(MAP[l]==MAP[k])
		d+=50;
	else
		d+=1;
	
	return d;
}

void Graph(int dist[numCities][numCities])
{
	int temp[6];
	temp[0]=SUB[0];

	for(int i=1;i<6;i++) {
		temp[i]=temp[i-1]+SUB[i];
	}

	for(int i=0;i<numCities;i++) {
		for(int j=i;j<numCities;j++) {
			if(j==i)
				dist[i][j]=0;
			else {
				int d=assign(i,j,temp);

				dist[i][j]=d;
				dist[j][i]=d;
			}

		}
	}

	// srand(time(NULL));
	// for (int i = 0; i < numCities; i++) {
	// 	for (int j = i; j < numCities; j++) {
	// 		if (j == i) {
	// 			dist[i][j] = 0;
	// 		} else {
	// 			int d = rand() % 8 + 1;
	// 			dist[i][j] = d;
	// 			dist[j][i] = d;
	// 		}
	// 	}
	// }
}

void printants(int ants[][numCities])
{
	for (int i = 0; i < numAnts; i++) {
		for (int j = 0; j < numCities; j++) {
			cout << ants[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}


void printdist(int dist[][numCities]) 
{
	for (int i = 0; i < numCities; i++) {
		for (int j = 0; j < numCities; j++) {
			cout << dist[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void printpheromones(double pheromones[][numCities])
{
	for (int i = 0; i < numCities; i++) {
		for (int j = 0; j < numCities; j++) {
			cout << pheromones[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void InitPheromones(double pheromones[][numCities])
{
	for (int i = 0; i < numCities; i++) {
		for (int j = 0; j < numCities; j++) {
			pheromones[i][j] = 0.01;
		}
	}
}

int IndexOfTarget(int target, int trail[])
{
	int i;
	for (i = 0; i < numCities; i++) {
		if (trail[i] == target) return i;
	}
	cout << "Target not Found in IndexOfTarget:   " << i << endl;
}

void InitAnts(int ants[numAnts][numCities])
{
	for (int i = 0; i < numAnts; i++) {
		int start = rand() % numCities;
		int trail[numCities];
		for (int j = 0; j < numCities; j++) {
			trail[j] = j;
		}
		random_shuffle(trail, trail + numCities);
		int index = IndexOfTarget(start, trail);
		swap(trail[0], trail[index]);
		for (int j = 0; j < numCities; j++) {
			ants[i][j] = trail[j];
		}
	}
}

double Length(int x[], int dist[][numCities])
{
	double ans = 0.0;
	for (int i = 0; i < numCities - 1; i++) {
		ans += dist[x[i]][x[i + 1]];
	}
	return ans;
}

void ShowLength(int ants[][numCities], int dist[][numCities])
{
	for (int i = 0; i < numAnts; i++) {
		cout << i << ": [ ";
		for (int j = 0; j < 4; j++) {
			cout << ants[i][j] << " ";
		}
		cout << " . . . ";
		for (int j = numCities - 4; j < numCities; j++) {
			cout << ants[i][j] << " ";
		}
		cout << "] len = ";
		double len = Length(ants[i], dist);
		cout << len << endl;
	}
}

void BestTrail(int ants[][numCities], int dist[][numCities], int bestTrail[])
{
	double bestlength = Length(ants[0], dist);
	int index = 0;
	for (int i = 1; i < numAnts; i++) {
		double len = Length(ants[i], dist);
		if (len < bestlength) {
			bestlength = len;
			index = i;
		}
	}

	for (int i = 0; i < numCities; i++) {
		bestTrail[i] = ants[index][i];
	}
}

void Probability(int k, int cityX, bool visited[], double pheromones[][numCities], int dist[][numCities], double probs[])
{
	double tau[numCities];
	double sum = 0.0;
	for (int i = 0; i < numCities; i++) {
		if (i == cityX) {
			tau[i] = 0.0;
		} else if (visited[i] == true) {
			tau[i] = 0.0;
		} else {
			tau[i] = (pow(pheromones[cityX][i], alpha*1.0)) * (pow((1.0 / dist[cityX][i]*1.0), beta*1.0));
			if (tau[i] < 0.0001) {
				tau[i] = 0.0001;
			} else if (tau[i] > DBL_MAX / numCities * 100) {
				tau[i] = DBL_MAX / numCities * 100;
			}
		}
		sum += tau[i];
	}
	for (int i = 0; i < numCities; i++) {
		probs[i] = tau[i] / sum;
	}
}

int NextCity(int k, int cityX, bool visited[], double pheromones[][numCities], int dist[][numCities])
{
	double probs[numCities];
	Probability(k, cityX, visited, pheromones, dist, probs);
	double cum[numCities + 1];
	cum[0] = 0.0;
	for (int i = 0; i < numCities; i++) {                  
		cum[i + 1] = cum[i] + probs[i];
	}
	double p = (double)rand() / (double)RAND_MAX;
	for (int i = 0; i < numCities; i++) {
		if (p >= cum[i] && p < cum[i + 1]) {
			return i;
		}
	}
	cout << "Failure\n";
}


void BuildTrail(int k, int start, double pheromones[][numCities], int dist[][numCities], int newTrail[])
{
	int trail[numCities];
	bool visited[numCities];
	for (int i = 0; i < numCities; i++) {
		visited[i] = false;
	}
	trail[0] = start;
	visited[start] = true;
	for (int i = 0; i < numCities - 1; i++) {
		int cityX = trail[i];
		int next = NextCity(k, cityX, visited, pheromones, dist);
		trail[i + 1] = next;
		visited[next] = true;
	}
	for (int i = 0; i < numCities; i++) {
		newTrail[i] = trail[i];
	}
}

void UpdateAnts(int ants[][numCities], double pheromones[][numCities], int dist[][numCities])
{
	for (int i = 0; i < numAnts; i++) {
		int start = rand() % numCities;
		int newTrail[numCities];
		BuildTrail(i, start, pheromones, dist, newTrail);
		for (int j = 0; j < numCities; j++) {
			ants[i][j] = newTrail[j];
		}
	}
}

bool EdgePresentinTrail(int cityX, int cityY, int trail[])
{
	int lastIndex = numCities - 1;
	int index = IndexOfTarget(cityX, trail);
	if (index == 0 && trail[1] == cityY) return true;
        else if (index == 0 && trail[lastIndex] == cityY) return true;
        else if (index == 0) return false;
        else if (index == lastIndex && trail[lastIndex - 1] == cityY) return true;
        else if (index == lastIndex && trail[0] == cityY) return true;
        else if (index == lastIndex) return false;
        else if (trail[index - 1] == cityY) return true;
        else if (trail[index + 1] == cityY) return true;
        else return false;
}

void UpdatePheromones(int ants[][numCities], double pheromones[][numCities], int dist[][numCities]) 
{
	for (int i = 0; i < numCities; i++) {
		for (int j = i + 1; j < numCities; j++) {
			for (int k = 0; k < numAnts; k++) {
				double length = Length(ants[k], dist);
				double decrease = (1.0 - rho) * pheromones[i][j];
				double increase = 0.0;
				if (EdgePresentinTrail(i, j, ants[k]) == true) {
					increase = Q / length;
				}
				pheromones[i][j] = increase + decrease;
				if (pheromones[i][j] < 0.0001) {
					pheromones[i][j] = 0.0001;
				} else if (pheromones[i][j] > 100000.0) {
					pheromones[i][j] = 100000.0;
				}
				pheromones[j][i] = pheromones[i][j];
			}
		}
	}
}

void Display(int besttrail[])
{
	for (int i = 0; i < numCities; i++) {
		cout << besttrail[i] << " ";
		if (i > 0 && i % 20 == 0) {
			cout << endl;
		}
	}
	cout << endl;

	int temp[6];
	temp[0]=SUB[0];

	for(int i=1;i<6;i++) {
		temp[i]=temp[i-1]+SUB[i];

	}

	for(int i=0;i<numCities;i++) {
		int j;
		for(j=0;j<6;j++) {
			if(besttrail[i]<temp[j])
				break;
		}

		if(i%6==0)
			cout<<"\n";
		
		cout<<"\t"<<SUBJECTS[j];
	}
}

int main()
{
	cout << "\nBegin Ant Colony Demo:\n\n";
	cout << "Number of cities in the problem: " << numCities << endl;
	cout << "Number of ants: " << numAnts << endl;
	cout << "Maximum Time: " << maxTime << endl;
	cout << "Alpha (pheromone influence): " << alpha << endl;
	cout << "Beta (local node influence): " << beta << endl;
	cout << "Rho (pheromone evaporation coefficient): " << rho << endl;
	cout << "Q (pheromone deposit factor) " << Q << endl;
	cout << "\nCreating Dummy Undirected Graph with Random edge lengths:\n";

	int dist[numCities][numCities];
	Graph(dist);                                           // Making Graph
	ofstream f;
	f.open("Output.txt");
	if (f.good()) {
		cout << "hello";
	}
	for (int i = 0; i < numCities; i++) {
		for (int j = 0; j < numCities; j++) {
			f << dist[i][j];
			f << " ";
		}
		f << endl;
	}

	cout << "\nInitialising ants to random trails:\n";	// Initialising trails to ants
	int ants[numAnts][numCities];
	InitAnts(ants);
	ShowLength(ants, dist);

	int bestTrail[numCities];
	BestTrail(ants, dist, bestTrail);
	
	double bestlength = Length(bestTrail, dist); 
	bestlength += dist[bestTrail[0]][bestTrail[numCities - 1]];

	cout << "\nBest Initial Trail Length: " << bestlength << endl << endl;

	cout << "\nInitialising Pheromones on trails: \n";
	double pheromones[numCities][numCities];      //Initialising Pheromones
	InitPheromones(pheromones);
	int time = 0;
	while (time < maxTime) {
		UpdateAnts(ants, pheromones, dist);
		UpdatePheromones(ants, pheromones, dist);
		int currBestTrail[numCities];
		BestTrail(ants, dist, currBestTrail);
		double currbestlength = Length(currBestTrail, dist);
		currbestlength += dist[currBestTrail[0]][currBestTrail[numCities - 1]];
		
		if (currbestlength < bestlength) {
			bestlength = currbestlength;
			for (int i = 0; i < numCities; i++) {
				bestTrail[i] = currBestTrail[i];
			}
			cout << "New Best Trail of: " << bestlength << " found at time : " << time << endl;
		}
		++time;
	}

	cout << "\nTime Complete:\n";
	cout << "\nBest Trail Found:\n";
	
	Display(bestTrail);
	
	cout << "\nLength of Best trail found: " << bestlength << endl;

	return 0;
}