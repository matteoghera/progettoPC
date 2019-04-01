#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>


//Pthreads libraries
#define _OPEN_THREADS

#include <pthread.h>


#define FALSE 0
#define TRUE 1
#define TOLL 0.01
#define NUM_PROCRESS 15


// stucture that represents each cluster.
struct Clusters {
    double cx;
    double cy;
    char *label;
    struct Clusters *next;
};

// stucture that represents a point of the dataset.
struct Points {
    double x;
    double y;
    struct Clusters *cluster;
    struct Points *next;
};

typedef struct Clusters Clusters;
typedef struct Points Points;


Points *head;
Clusters *clusters;
int numberOfPoints = 0, k = 0;
int dimensionOfChunk;
long timeIO, executionTime;


double euclideanDistance(Clusters *currentCluster, Points *currentPoint) {
    /*
    * computes euclideanDistance between the current point and centroid.
    */
    double x1 = currentCluster->cx;
    double y1 = currentCluster->cy;
    double x2 = currentPoint->x;
    double y2 = currentPoint->y;

    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void *MakeCluster(void *input) { //parallelized
    /*
     *  assigns records to the nearest cluster.
    */
    Points *nextRecord;
    nextRecord = (Points *) input;

    double **myClusterDescription;
    int righe = k;
    int colonne = 3;
    int r, c;
    myClusterDescription = (double **) malloc((righe) * sizeof(double));
    for (r = 0; r < colonne; r++){
        myClusterDescription[r] = (double *) malloc((colonne) * sizeof(double));
        myClusterDescription[r][0]=0;
        myClusterDescription[r][1]=0;
        myClusterDescription[r][2]=0;
    }


    int endPoints = FALSE;
    int i = 0;
    while (i < dimensionOfChunk && endPoints == FALSE) {
        double min = 10000;
        Clusters *minCluster;
        minCluster = clusters;
        int clusterIndex = 0;

        Clusters *nextCluster;
        nextCluster = clusters;
        int endCluster = FALSE;
        int c = 0;
        while (endCluster == FALSE) {
            double distance = euclideanDistance(nextCluster, nextRecord);
            if (distance < min) {
                min = distance;
                minCluster = nextCluster;
                clusterIndex = c;
            }
            if (nextCluster->next == NULL) {
                endCluster = TRUE;
            } else {
                c++;
                nextCluster = nextCluster->next;
            }
        }
        nextRecord->cluster = minCluster;
        myClusterDescription[clusterIndex][0] += nextRecord->x;
        myClusterDescription[clusterIndex][1] += nextRecord->y;
        myClusterDescription[clusterIndex][2]= myClusterDescription[clusterIndex][2]+1;

        if (nextRecord->next == NULL)
            endPoints = TRUE;
        else {
            nextRecord = nextRecord->next;
            i++;
        }
    }
    pthread_exit((void *) myClusterDescription);
}


int computesAverangeForAllClusters(double **actualClusterDescription) {
    /*
     * computes the centroid as the average of all the points of each cluster, to minimize SSE.
     */
    int result = FALSE;
    Clusters *nextCluster = clusters;
    for (int i = 0; i < k; i++) {
        double newCenterX, newCenterY;
        newCenterX = actualClusterDescription[i][0] / actualClusterDescription[i][2];
        newCenterY = actualClusterDescription[i][1] / actualClusterDescription[i][2];
        double diffX = newCenterX - nextCluster->cx;
        double diffY = newCenterY - nextCluster->cy;
        if ((diffX > TOLL * (-1) && diffX < TOLL) && (diffY > TOLL * (-1) && diffY < TOLL)) {
            result = FALSE;
        } else {
            result = TRUE;
            nextCluster->cx = newCenterX;
            nextCluster->cy = newCenterY;
        }
        nextCluster = nextCluster->next;
    }
    return result;
}

Points *assignChuncks(Points *actualRecord) {
    Points *nextRecord = actualRecord;
    int i = 0;
    while (i < dimensionOfChunk && nextRecord->next != NULL) {
        nextRecord = nextRecord->next;
        i++;
    }
    return nextRecord;
}

void mergeSolution(double **clusterDescription, double **actualCluster) {
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < 3; j++) {
            clusterDescription[i][j] += actualCluster[i][j];
        }
    }
}

int parallelKMeans() {
    // initialize the first chunk to the head of the Point structure
    Points *headChunk = head->next;

    //create a matrix that represents clusters created by each thread with its points
    double **clusterDescription;
    int righe = k;
    int colonne = 3;
    int r, c;
    clusterDescription = (double **) malloc((righe) * sizeof(double));
    for (r = 0; r < colonne; r++){
        clusterDescription[r] = (double *) malloc((colonne) * sizeof(double));
        clusterDescription[r][0]=0;
        clusterDescription[r][1]=0;
        clusterDescription[r][2]=0;
    }


    pthread_t threads[NUM_PROCRESS];
    short i;

    for (i = 0; i < NUM_PROCRESS; i++) {
        pthread_create(&threads[i], NULL, MakeCluster, (void *) headChunk);
        headChunk = assignChuncks(headChunk);
    }

    for (i = 0; i < NUM_PROCRESS; i++) {
        double **myClusterDescription;
        pthread_join(threads[i], (void *) &myClusterDescription);


        mergeSolution(clusterDescription, myClusterDescription);

    }

    return computesAverangeForAllClusters(clusterDescription);

}

void kMeans() {
    /*
     * implements kmeans algorithm.
     */
    int i = 0;
    int changed = TRUE;
    clusters = clusters->next;
    while (changed == TRUE) {
        i++;
        changed = parallelKMeans(); //parallelized
    }
    printf("Number Of Iterations: %d\n", i);
}

void printPoint() {
    /*
     * prints centroid for each cluster and the dataset points labeled with the name of the cluster to which they belong.
     */
    Clusters *nextCluster = clusters;
    int endCluster = FALSE;
    int i = 1;
    while (endCluster == FALSE) {
        printf("Centroid %d: %f, %f ", i, nextCluster->cx, nextCluster->cy);
        i++;
        if (nextCluster->next == NULL)
            endCluster = TRUE;
        else
            nextCluster = nextCluster->next;

    }
    printf("\n");

    Points *nextRecord = head;
    int fine = FALSE;
    while (fine == FALSE) {
        Clusters *cluster;
        cluster = nextRecord->cluster;
        if (cluster == NULL)
            printf("%f, %f, NULL\n", nextRecord->x, nextRecord->y);
        else
            printf("%f, %f, %s\n", nextRecord->x, nextRecord->y, cluster->label);

        if (nextRecord->next == NULL)
            fine = TRUE;
        else
            nextRecord = nextRecord->next;
    }
}

void addRecord(char record[]) {
    /*
     * inserts data into the Points structure.
     */
    numberOfPoints++;

    Points *nextRecord = head;
    while (nextRecord->next != NULL) {
        nextRecord = nextRecord->next;
    }

    Points *newRecord;
    newRecord = malloc(sizeof(Points));
    newRecord->x = 0;
    newRecord->y = 0;
    newRecord->next = NULL;
    newRecord->cluster = NULL;

    nextRecord->next = newRecord;


    char *tok = strtok(record, ",");
    double x = atof(tok);

    tok = strtok(NULL, ",");
    double y = atof(tok);

    newRecord->x = x;
    newRecord->y = y;
}

void addCentroid(char record[]) {
    /*
     * inserts data into the Clusters structure.
     */
    k++;

    Clusters *nextRecord = clusters;
    while (nextRecord->next != NULL) {
        nextRecord = nextRecord->next;
    }

    Clusters *newRecord;
    newRecord = malloc(sizeof(Clusters));
    newRecord->cx = 0;
    newRecord->cy = 0;
    newRecord->next = NULL;
    newRecord->label = NULL;

    nextRecord->next = newRecord;


    char *tok = strtok(record, ",");
    double x = atof(tok);

    tok = strtok(NULL, ",");
    double y = atof(tok);

    tok = strtok(NULL, "\n");
    newRecord->cx = x;
    newRecord->cy = y;
    newRecord->label = tok;
}

void parseCSV(char *pathfile) {
    /*
     * reads a .csv file that contains data line by line and saves them.
    */
    FILE *fd;

    //initialize the head of the structure containing all the points of the dataset
    head = malloc(sizeof(Points));
    head->x = 0;
    head->y = 0;
    head->next = NULL;
    head->cluster = NULL;

    // open .csv file
    fd = fopen(pathfile, "r");
    if (fd == NULL) {
        perror("Errore in apertura del file");
        exit(1);
    }

    // read each line
    while (1) {
        ssize_t res;
        char *buf = NULL;
        size_t len = 0;
        res = getline(&buf, &len, fd);
        if (res == -1) {
            //the file ended
            break;
        }
        //create a new record and insert it into the Point structure
        addRecord(buf);
    }
    //close .csv file
    fclose(fd);
}


void readCentroid(char *pathfile) {
    /*
     * reads a .csv file that contains the coordinates of centroid.
    */
    FILE *fd;

    //initialize the head of the structure containing clusters
    clusters = malloc(sizeof(Clusters));
    clusters->cx = 0;
    clusters->cy = 0;
    clusters->next = NULL;
    clusters->label = NULL;

    //open file
    fd = fopen(pathfile, "r");
    if (fd == NULL) {
        perror("Errore in apertura del file");
        exit(1);
    }
    int i = 0;

    while (1) {
        ssize_t res;
        char *buf = NULL;
        size_t len = 0;
        res = getline(&buf, &len, fd);
        if (res == -1) {
            break;
        }
        //create a new cluster and insert it into the Cluster structure
        addCentroid(buf);
    }
    //close file
    fclose(fd);
}

void printStatistics(){
    printf("\n\n\n----------------------------------------------------------------------");
    printf("\n----------------------- STATISTICS -----------------------------------");
    printf("\n----------------------------------------------------------------------\n\n");

    printf("Program Type:     \tParallel\n");
    printf("Number Of Threads:\t%d\n", NUM_PROCRESS);

    long totalTime=timeIO+executionTime;

    printf("I/O time:         \t%ld ns\t(%f%%)\n", timeIO, ((double)timeIO/(double)totalTime)*100);
    printf("Execution Time:   \t%ld ns\t(%f%%)\n", executionTime, ((double) executionTime/(double)totalTime)*100);
    printf("----------------------------------------------------------------------\n");
    printf("Total time:       \t%ld ns\t(100%%)\n", totalTime);


    printf("----------------------------------------------------------------------\n\n");
}

int main() {
    //read the file
    //the files are in the CMakeFiles folder
    struct timeval start, stop;
    gettimeofday(&start,NULL);

    printf("Parallel program execution:\n\n");
    char *initialCentroidsFileName = "/datasetCentroids.csv";
    char *pointsFileName = "/rectangularPoints.csv";

    char description[100];
    char *currentPath = getcwd(description, sizeof(description));
    char path_points[100], path_centroids[100];
    char *pathFileCentroids = strcpy(path_points, currentPath);
    char *pathFilePoints = strcpy(path_centroids, currentPath);
    pathFileCentroids = strcat(pathFileCentroids, initialCentroidsFileName);
    pathFilePoints = strcat(pathFilePoints, pointsFileName);
    printf("CSV paths: %s, %s\n", pathFileCentroids, pathFilePoints);

    //save the coordinates into the structure Point
    parseCSV(pathFilePoints);

    //create the clusters and read centroid
    readCentroid(pathFileCentroids);

    gettimeofday(&stop, NULL);
    timeIO=( ((stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));





    gettimeofday(&start,NULL);

    //create data chunk for threads
    dimensionOfChunk = (int) ceil(((double) numberOfPoints) / ((double) NUM_PROCRESS));

    kMeans(); //parallelized

    gettimeofday(&stop, NULL);
    executionTime=( ((stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));




    //gettimeofday(&start,NULL);

    //printPoint();

    //gettimeofday(&stop, NULL);
    //timeIO=( ((stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));


    printStatistics();
}
