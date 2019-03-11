#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define FALSE 0
#define TRUE 1
#define TOLL 0.01

// stucture that represents each cluster.
struct Clusters {
    double cx;
    double cy;
    char* label;
    struct Clusters* next;
};

// stucture that represents a point of the dataset.
struct Points{
    double x;
    double y;
    struct Clusters *cluster;
    struct Points *next;

};



typedef struct Clusters Clusters;
typedef struct Points Points;


Points* head;
Clusters* clusters;
long timeIO, executionTime;




double euclideanDistance(Clusters* currentCluster, Points* currentPoint){
    /*
    * computes euclideanDistance between the current point and centroid.
    */
    double x1=currentCluster->cx;
    double y1=currentCluster->cy;
    double x2=currentPoint->x;
    double y2=currentPoint->y;

    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

void MakeCluster() {
    /*
     *  assigns records to the nearest cluster.
    */
    Points* nextRecord=head->next;
    int endPoints=FALSE;
    while(endPoints==FALSE) {
        double min=10000;
        Clusters* minCluster;
        minCluster=clusters;

        Clusters* nextCluster=clusters;
        int endCluster=FALSE;
        while(endCluster==FALSE) {
            double distance=euclideanDistance(nextCluster, nextRecord);
            if(distance<min){
                min=distance;
                minCluster=nextCluster;
            }
            if (nextCluster->next == NULL)
                endCluster = TRUE;
            else
                nextCluster = nextCluster->next;

        }
        nextRecord->cluster=minCluster;


        if (nextRecord->next == NULL)
            endPoints = TRUE;
        else
            nextRecord = nextRecord->next;
    }
}

int computesAverangeForAllClusters() {
    /*
     * computes the centroid as the average of all the points of each cluster, to minimize SSE.
     */
    int result=TRUE;

    Clusters* nextCluster=clusters;
    int endCluster=FALSE;
    while(endCluster==FALSE) {
        double sumX=0, sumY=0;
        int nPoints=0;

        Points* nextRecord=head->next;
        int endPoints=FALSE;
        while(endPoints==FALSE) {
            Clusters *point_cluster;
            point_cluster = nextRecord->cluster;
            if(strcmp(point_cluster->label, nextCluster->label)==0){
                sumX += nextRecord->x;
                sumY += nextRecord->y;
                nPoints++;
            }

            if (nextRecord->next == NULL)
                endPoints = TRUE;
            else
                nextRecord = nextRecord->next;
        }
        double newCenterX=sumX/nPoints;
        double newCenterY=sumY/nPoints;
        double diffX=newCenterX-nextCluster->cx;
        double diffY=newCenterY-nextCluster->cy;
        if((diffX>TOLL*(-1) && diffX<TOLL)&&(diffY>TOLL*(-1) && diffY<TOLL)){
            result=FALSE;
        } else {
            result=TRUE;
            nextCluster->cx=newCenterX;
            nextCluster->cy=newCenterY;
        }

        if (nextCluster->next == NULL)
            endCluster = TRUE;
        else
            nextCluster = nextCluster->next;

    }
    return result;
}

void kMeans(){
    /*
     * implements kmeans algorithm.
     */
    int i=0;
    int changed=TRUE;
    clusters=clusters->next;
    while (changed==TRUE){
        i++;
        MakeCluster();
        changed=computesAverangeForAllClusters();
    }
    printf("Number Of Iterations: %d\n", i);
}

void printPoint(){
    /*
     * prints centroid for each cluster and the dataset points labeled with the name of the cluster to which they belong.
     */
    Clusters* nextCluster=clusters;
    int endCluster=FALSE;
    int i=1;
    while(endCluster==FALSE) {
        printf("Centroid %d: %f, %f ", i, nextCluster->cx, nextCluster->cy);
        i++;
        if (nextCluster->next == NULL)
            endCluster = TRUE;
        else
            nextCluster = nextCluster->next;

    }
    printf("\n");

    Points* nextRecord=head;
    int fine=FALSE;
    while(fine==FALSE) {
        Clusters* cluster;
        cluster=nextRecord->cluster;
        if (cluster==NULL)
            printf("%f, %f, NULL\n", nextRecord->x, nextRecord->y);
        else
            printf("%f, %f, %s\n", nextRecord->x, nextRecord->y, cluster->label);

        if (nextRecord->next==NULL)
            fine=TRUE;
        else
            nextRecord = nextRecord->next;
    }
}

void addRecord(char record[]){
    /*
     * inserts data into the Points structure.
     */
    Points* nextRecord=head;
    while(nextRecord->next!=NULL) {
        nextRecord = nextRecord->next;
    }

    Points* newRecord;
    newRecord=malloc(sizeof(Points));
    newRecord->x=0;
    newRecord->y=0;
    newRecord->next=NULL;
    newRecord->cluster=NULL;

    nextRecord->next=newRecord;


    char * tok =strtok(record, ",");
    double x=atof(tok);

    tok =strtok(NULL, ",");
    double y=atof(tok);

    newRecord->x=x;
    newRecord->y=y;
}

void addCentroid(char record[]){
    /*
     * inserts data into the Clusters structure.
     */
    Clusters* nextRecord=clusters;
    while(nextRecord->next!=NULL) {
        nextRecord = nextRecord->next;
    }

    Clusters* newRecord;
    newRecord=malloc(sizeof(Clusters));
    newRecord->cx=0;
    newRecord->cy=0;
    newRecord->next=NULL;
    newRecord->label=NULL;

    nextRecord->next=newRecord;


    char * tok =strtok(record, ",");
    double x=atof(tok);

    tok =strtok(NULL, ",");
    double y=atof(tok);

    tok=strtok(NULL,",\n");
    newRecord->cx=x;
    newRecord->cy=y;
    newRecord->label=tok;
}

void parseCSV(char* pathfile) {
    /*
     * reads a .csv file that contains data line by line and saves them.
    */
    FILE *fd;

    head=malloc(sizeof(Points));
    head->x=0;
    head->y=0;
    head->next=NULL;
    head->cluster=NULL;

    fd=fopen(pathfile, "r");
    if( fd==NULL ) {
        perror("Errore in apertura del file");
        exit(1);
    }

    /* legge ogni riga */
    while(1) {
        ssize_t res;
        char *buf=NULL;
        size_t len=0;
        res=getline(&buf, &len, fd);
        if (res==-1){
            break;
        }
        addRecord(buf);
    }
    fclose(fd);
}


void readCentroid(char* pathfile) {
    /*
     * reads a .csv file that contains the coordinates of centroid.
    */
    FILE *fd;
    clusters=malloc(sizeof(Clusters));
    clusters->cx=0;
    clusters->cy=0;
    clusters->next=NULL;
    clusters->label=NULL;

    fd=fopen(pathfile, "r");
    if( fd==NULL ) {
        perror("Errore in apertura del file");
        exit(1);
    }
    int i=0;

    while(1) {
        ssize_t res;
        char *buf=NULL;
        size_t len=0;
        res=getline(&buf, &len, fd);
        if (res==-1){
            break;
        }
        addCentroid(buf);
    }
    fclose(fd);
}

void printStatistics(){
    printf("\n\n\n----------------------------------------------------------------------");
    printf("\n----------------------- STATISTICS -----------------------------------");
    printf("\n----------------------------------------------------------------------\n\n");

    printf("Program Type:\t\tSequential\n");
    printf("Number Of Threads:\t1\n");

    long totalTime=timeIO+executionTime;

    printf("I/O time:\t\t\t%ld ns\t(%f%%)\n", timeIO, ((double)timeIO/(double)totalTime)*100);
    printf("Execution Time:\t\t%ld ns\t\t(%f%%)\n", executionTime, ((double) executionTime/(double)totalTime)*100);
    printf("----------------------------------------------------------------------\n");
    printf("Total time:\t\t\t%ld ns\t(100%%)\n", totalTime);


    printf("----------------------------------------------------------------------\n\n");
}


int main() {
    //read the file
    //the files are in the CMakeFiles folder
    struct timeval start, stop;
    gettimeofday(&start,NULL);

    printf("Sequential program execution: \n\n");
    char* initialCentroidsFileName="/initialCentroids.csv";
    char* pointsFileName="/points.csv";

    char description[100];
    char* currentPath=getcwd(description, sizeof(description));
    char path_points[100], path_centroids[100];
    char * pathFileCentroids=strcpy(path_points, currentPath);
    char* pathFilePoints=strcpy(path_centroids, currentPath);
    pathFileCentroids=strcat(pathFileCentroids,initialCentroidsFileName);
    pathFilePoints=strcat(pathFilePoints,pointsFileName);
    printf("CSV paths: %s, %s\n", pathFileCentroids, pathFilePoints);


    //save the coordinates into the structure Point
    parseCSV(pathFilePoints);

    //create the clusters and read centroid
    readCentroid(pathFileCentroids);

    gettimeofday(&stop, NULL);
    timeIO=( ((stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));




    gettimeofday(&start,NULL);

    //runs kmeans
    kMeans();

    gettimeofday(&stop, NULL);
    executionTime=( ((stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));



    //gettimeofday(&start,NULL);

    //printPoint();

    //gettimeofday(&stop, NULL);
    //timeIO=( ((stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

    printStatistics();
}
