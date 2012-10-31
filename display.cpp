/**
 *
 * @author praveen_kulkarni
 */


#include "header.h"

static bool fullscreen = false;
static bool mouseDown = false;

static float xrot = -90.0f;
static float yrot = 0.0f;
static float zrot = 0.0f;

static float xdiff = 0.0f;
static float ydiff = 0.0f;

static float zoomvalue = 5.0f;
static float particlesize = PARTICLE_SIZE;
static int step = -1;

static const int maxClusters = NUMBEROFCLUSTERS;
static const int numOfSegmentsToDrawCircle = NUMOFSEGMENTSTODRAWCIRCLE;
static int currentClusterCenterIndex=0;
static int clusterCenters[maxClusters];
static int innerClusterCircleFlag[maxClusters];
static int outerClusterCircleFlag[maxClusters];

static int numberOfMarkerClusterCircles = 1;


void populateRoutes(){

	for(int count=0; count<maxClusters; count++){
		clusterCenters[count] = -1;
		innerClusterCircleFlag[count] = 0;
		outerClusterCircleFlag[count] = 0;
	}

	for(int count=0; count<NUMBEROFPOINTS; count++){
		allRoutesCoOrdinatesZ[count] = 0.0f;
	}

	int pointFinishedIndex = 0;

	/// 10% random points
	srand(time(NULL));
	for(int count=0; count<NUMBEROFPOINTS/10; count++){
		allRoutesCoOrdinatesX[count] = (float)rand()/(float)RAND_MAX;
		allRoutesCoOrdinatesY[count] = (float)rand()/(float)RAND_MAX;
		pointFinishedIndex++;
	}

	// generating cluster centers
	srand(time(NULL));
	for(int count=0; count<NUMBEROFCLUSTERS; count++){
		clusterWidth[count] = ((float)rand()/(float)RAND_MAX)*0.6f+0.2f;
		clusterCenterX[count] = ((float)rand()/(float)RAND_MAX)*(1.0f-clusterWidth[count])+(clusterWidth[count]*0.5f);
		clusterCenterY[count] = ((float)rand()/(float)RAND_MAX)*(1.0f-clusterWidth[count])+(clusterWidth[count]*0.5f);
	}

	// maximum points in multiple of NUMBEROFCLUSTERS
	int remainingPoints = NUMBEROFPOINTS - pointFinishedIndex;
	int numOfPointsPerCluster = remainingPoints/NUMBEROFCLUSTERS;

	// fill clusters
	int temp = pointFinishedIndex;
	for(int count=0; count<NUMBEROFCLUSTERS; count++){
		float width = clusterWidth[count];
		srand(time(NULL));
		for(int count1=0; count1<numOfPointsPerCluster; count1++){
			int index = temp+(count*numOfPointsPerCluster)+count1;
			if(count1<numOfPointsPerCluster/5){
				allRoutesCoOrdinatesX[index]=
					((float)rand()/(float)RAND_MAX)*(width)+
					(clusterCenterX[count]-(width*0.5f));
				allRoutesCoOrdinatesY[index]=
					((float)rand()/(float)RAND_MAX)*(width)+
					(clusterCenterY[count]-(width*0.5f));
			}else if(count1<numOfPointsPerCluster/4){
				float temp_width = width*(float)rand()/(float)RAND_MAX;
				allRoutesCoOrdinatesX[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterX[count]-(temp_width*0.5f));
				allRoutesCoOrdinatesY[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterY[count]-(temp_width*0.5f));
			}else if(count1<numOfPointsPerCluster/3){
				float temp_width = width*(float)rand()/(float)RAND_MAX;
				allRoutesCoOrdinatesX[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterX[count]-(temp_width*0.5f));
				allRoutesCoOrdinatesY[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterY[count]-(temp_width*0.5f));
			}else if(count1<numOfPointsPerCluster/2){
				float temp_width = width*(float)rand()/(float)RAND_MAX;
				allRoutesCoOrdinatesX[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterX[count]-(temp_width*0.5f));
				allRoutesCoOrdinatesY[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterY[count]-(temp_width*0.5f));
			}else{
				float temp_width = width*(float)rand()/(float)RAND_MAX;
				allRoutesCoOrdinatesX[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterX[count]-(temp_width*0.5f));
				allRoutesCoOrdinatesY[index]=
					((float)rand()/(float)RAND_MAX)*(temp_width)+
					(clusterCenterY[count]-(temp_width*0.5f));
			}
			if(allRoutesCoOrdinatesX[index]<=0.0f){
				allRoutesCoOrdinatesX[index]=0.00001f;
			}else if(allRoutesCoOrdinatesX[index]>=1.0f){
				allRoutesCoOrdinatesX[index]=0.99999f;
			}

			if(allRoutesCoOrdinatesY[index]<=0.0f){
				allRoutesCoOrdinatesY[index]=0.00001f;
			}else if(allRoutesCoOrdinatesY[index]>=1.0f){
				allRoutesCoOrdinatesY[index]=0.99999f;
			}
			pointFinishedIndex++;
		}
	}

	// fill remaining points
	srand(time(NULL));
	for(int count = pointFinishedIndex; count < NUMBEROFPOINTS; count++){
		allRoutesCoOrdinatesX[count] = (float)rand()/(float)RAND_MAX;
		allRoutesCoOrdinatesY[count] = (float)rand()/(float)RAND_MAX;
		pointFinishedIndex++;
	}

	// shuffle the points in array
    if (NUMBEROFPOINTS > 1) {
        int i;
		for (i = 0; i < NUMBEROFPOINTS - 1; i++) {
		  int j = i + rand() / (RAND_MAX / (NUMBEROFPOINTS - i) + 1);
		  float tx = allRoutesCoOrdinatesX[j];
		  float ty = allRoutesCoOrdinatesY[j];
		  allRoutesCoOrdinatesX[j] = allRoutesCoOrdinatesX[i];
		  allRoutesCoOrdinatesY[j] = allRoutesCoOrdinatesY[i];
		  allRoutesCoOrdinatesX[i] = tx;
		  allRoutesCoOrdinatesY[i] = ty;
		}
    }
}

bool init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
	//glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	return true;
}

void modifyPotentialZ(){
	size_t i,j=0;

	for(i=0;i<NUMBEROFPOINTS;i++){
		float temp = allRoutesCoOrdinatesZ[i];
		float tempx = allRoutesCoOrdinatesX[i];
		float tempy = allRoutesCoOrdinatesY[i];
		for(j=0;j<NUMBEROFPOINTS;j++){
			temp += exp(-ALPHA*(pow(tempx-allRoutesCoOrdinatesX[j],2)+pow(tempy-allRoutesCoOrdinatesY[j],2)));
		}
		allRoutesCoOrdinatesZ[i] = temp;
	}
	
	//find max and normalize
	float maxOfAll = -1.0f;
	for (i=0; i<NUMBEROFPOINTS; i++)
	{
		 if (allRoutesCoOrdinatesZ[i]>maxOfAll)
		 {
			maxOfAll=allRoutesCoOrdinatesZ[i];
			clusterCenters[0]=i;
		 }
	}
	for (i=0; i<NUMBEROFPOINTS; i++)
	{
		allRoutesCoOrdinatesZ[i]/=maxOfAll;
	}
}

void modifyOuterCirclePotentialZAndEstimateNextClusterCenter(){
	size_t i=0;
	float cx = allRoutesCoOrdinatesX[clusterCenters[currentClusterCenterIndex]];
	float cy = allRoutesCoOrdinatesY[clusterCenters[currentClusterCenterIndex]];
	float cz = allRoutesCoOrdinatesZ[clusterCenters[currentClusterCenterIndex]];
	for(i=0;i<NUMBEROFPOINTS;i++){
		//float sqrdist = pow(allRoutesCoOrdinatesX[i]-cx,2) + pow(allRoutesCoOrdinatesY[i]-cy,2);
		//if(pow(OUTERRADIUS,2)>sqrdist){
		//	allRoutesCoOrdinatesZ[i]=0.0f;
		//	printf("--%d (%f,%f)<(%f,%f)",i,allRoutesCoOrdinatesX[i],allRoutesCoOrdinatesY[i],cx,cy);
		//}
		allRoutesCoOrdinatesZ[i]-= cz*exp(-BETA*(pow(allRoutesCoOrdinatesX[i]-cx,2)+pow(allRoutesCoOrdinatesY[i]-cy,2)));
	}

	//find next max
	float max = -1.0f;
	if(currentClusterCenterIndex<maxClusters-1){
		for (i=0; i<NUMBEROFPOINTS; i++)
		{
			if (allRoutesCoOrdinatesZ[i]>max)
			{
				max=allRoutesCoOrdinatesZ[i];
				clusterCenters[currentClusterCenterIndex+1]=i;
			}
		}
	}
}

void printPoints(){
	printf("/n||||||\n");
	for(int i=0;i<NUMBEROFPOINTS;i++){
		printf("index:%d cx:%f cy:%f cz:%f\n",i,allRoutesCoOrdinatesX[i],allRoutesCoOrdinatesY[i],allRoutesCoOrdinatesZ[i]);
	}
	printf("/n||||||\n");
}

void drawClusterBox()
{
	float size_box = 2.0f;

	///////////////////// wire cube
	glColor3f(0.9f,0.1f,0.7f);
	glutWireCube(size_box);

	////////////////// center cube
	//glColor3f(0.0,1.0,0.0);
	//glutSolidCube(size_box/10.0f);

	/////////// clip plane
	/*
	if(sliceplanestatus[0]==1){
		glClipPlane (GL_CLIP_PLANE0, eqn[0]);
		glEnable (GL_CLIP_PLANE0);
	}
	*/
	
	////////////// fill by points
	glPointSize(particlesize);
	//glEnable(GL_POINT_SMOOTH);
	//glColor3f(0.0,0.0,0.0);
	glTranslatef(-1.0f,-1.0f,-1.0f);
	glScalef(2.0f,2.0f,2.0f);

	glPushMatrix();
	glBegin(GL_POINTS);	
	for(int i=0; i<NUMBEROFPOINTS; i++){
		float z = allRoutesCoOrdinatesZ[i];
		glColor3f(z,1.0-z,0.0);
		glVertex3f(allRoutesCoOrdinatesX[i],allRoutesCoOrdinatesY[i],z);
	}
	glEnd();
	glPopMatrix();
	
	///////////////////////
	/*
	if(sliceplanestatus[0]==1){
		glDisable (GL_CLIP_PLANE0);
	}
	*/
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0,0,-zoomvalue);
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f,1.0f,0.5f);

	/////////////// zoom
	//glPushMatrix();
	drawClusterBox();
	//glPopMatrix();

	// draw circles
	for(int mccount=0; mccount<numberOfMarkerClusterCircles; mccount++){
		float cz = 0.1f*mccount*0.1f;

		for(int c = 0; c<maxClusters; c++){
			float cx,cy;
			cx=allRoutesCoOrdinatesX[clusterCenters[c]];
			cy=allRoutesCoOrdinatesY[clusterCenters[c]];
			if(innerClusterCircleFlag[c]==1){
				glColor3f(0.6,0.05,0.45);
				glBegin(GL_LINE_LOOP); 
				for(int ii = 0; ii < numOfSegmentsToDrawCircle; ii++) 
				{ 
					float theta = 2.0f * 3.1415926f * float(ii) / float(numOfSegmentsToDrawCircle);//get the current angle 

					float x = RADIUS * cosf(theta);//calculate the x component 
					float y = RADIUS * sinf(theta);//calculate the y component 

					glVertex3f(x + cx, y + cy,1.0f-cz);//output vertex 

				} 
				glEnd(); 
			}

			if(outerClusterCircleFlag[c]==1){
				glColor3f(0.6,0.7,0.3);
				glBegin(GL_LINE_LOOP); 
				for(int ii = 0; ii < numOfSegmentsToDrawCircle; ii++) 
				{ 
					float theta = 2.0f * 3.1415926f * float(ii) / float(numOfSegmentsToDrawCircle);//get the current angle 

					float x = OUTERRADIUS * cosf(theta);//calculate the x component 
					float y = OUTERRADIUS * sinf(theta);//calculate the y component 

					glVertex3f(x + cx, y + cy,1.0f-cz);//output vertex 

				} 
				glEnd(); 
			}
		}
	}
	

	


	glFlush();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(45.0f, 1.0f * w / h, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
	if (!mouseDown)
	{
		zrot += 0.2f;
		//yrot += 0.15f;
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{

	///// < decrease number of marker circles
	case 60:
		numberOfMarkerClusterCircles-=1;
		if(numberOfMarkerClusterCircles<1)numberOfMarkerClusterCircles=1;
		break;

	///// > decrease number of marker circles
	case 62:
		numberOfMarkerClusterCircles+=1;
		if(numberOfMarkerClusterCircles>100)numberOfMarkerClusterCircles=100;
		break;

	//// P increase particle size
	case 80:
		particlesize += 1.0f;
		if(particlesize>7.0f)particlesize=7.0f;
		break;

	//// p increase particle size
	case 112:
		particlesize -= 1.0f;
		if(particlesize<0.0f)particlesize=0.0f;
		break;
	
	/////////// escape
	case 27 : 
		exit(1); break;
	}
}

void specialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		fullscreen = !fullscreen;

		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(500, 500);
			glutPositionWindow(50, 50);
		}
	}

	switch(key)
	{


	//////////////
	case GLUT_KEY_DOWN:
		zoomvalue += 0.2f;
		if(zoomvalue>7.0f)zoomvalue=7.0f;
		break;

	case GLUT_KEY_UP:
		zoomvalue -= 0.2f;
		if(zoomvalue<1.0f)zoomvalue=1.0f;
		break;

	case GLUT_KEY_HOME:
		zoomvalue = 5.0f;
		xrot = -90.0f;
		yrot = 0.0f;
		zrot = 0.0f;
		//particlesize = PARTICLE_SIZE;
		break;

	case GLUT_KEY_RIGHT:
		if(currentClusterCenterIndex>=maxClusters){
			printf("\nclusters exceeded \n");
		}else{
		
			if(step==-1){
				printf("\nfirst step initialize all potentials...\n||||||||||||||||||\n");
				modifyPotentialZ();
			}

			if(step==0){
				printf("\nCluster %d detected at (%f,%f)\n",currentClusterCenterIndex+1,allRoutesCoOrdinatesX[clusterCenters[currentClusterCenterIndex]],allRoutesCoOrdinatesY[clusterCenters[currentClusterCenterIndex]]);
				innerClusterCircleFlag[currentClusterCenterIndex]=1;
				printf("display inner circle\n");
			}
			if(step==1){
				outerClusterCircleFlag[currentClusterCenterIndex]=1;
				printf("display outer circle\n");
			}
			if(step==2){
				modifyOuterCirclePotentialZAndEstimateNextClusterCenter();
				printf("drop the potentials in marked cluster\n");
			}

			step++;
			if(step>2){
				step=0;
				currentClusterCenterIndex++;
			}
		}
		
		
		
		break;

	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;

		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
}

void mouseMotion(int x, int y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		xrot = y + ydiff;

		glutPostRedisplay();
	}
}

int clusterDisplay(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE| GLUT_DEPTH);
	
	glutCreateWindow("Praveen Cluster");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	if (!init())
		return 1;

	glutMainLoop();

	return 0;
}




int main(int argc, char *argv[])
{
	populateRoutes();

	clusterDisplay(argc, argv);


	//_getch();


}










