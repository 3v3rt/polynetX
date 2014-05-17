#include "Generator.h"
#include "Mersenne.h"

using namespace std;

double        **allocate_2_double (int xdim, int ydim)
{
	int             i;
	double        **grid;
	grid = (double **) malloc (xdim * sizeof (double *));
	if (grid == NULL)
	{
		printf ("OUT OF MEMORY\n");
		exit (1);
	}
	grid[0] = (double *) malloc (ydim * xdim * sizeof (double));
	if (grid[0] == NULL)
	{
		printf ("OUT OF MEMORY\n");
		exit (1);
	}
	for (i = 1; i < xdim; i++)
		grid[i] = grid[i - 1] + ydim;
	return grid;
}

void free_2_double(double** ppGrid)
{
	free(ppGrid[0]);
	free(ppGrid);
}

int          ***allocate_3d (int xdim, int ydim, int zdim)
{
	int             i;
	int          ***grid;

	//printf("Allocating [%d %d %d]\n", xdim, ydim, zdim);

	grid = (int ***) malloc (xdim * sizeof (int **));
	if (grid == NULL)
	{
		printf ("OUT OF MEMORY\n");
		exit (1);
	}

	grid[0] = (int **) malloc (ydim * xdim * sizeof (int *));
	if (grid[0] == NULL)
	{
		printf ("OUT OF MEMORY\n");
		exit (1);
	}

	for (i = 1; i < xdim; i++)
		grid[i] = grid[i - 1] + ydim;

	grid[0][0] = (int *) malloc (zdim * ydim * xdim * sizeof (int));
	if (grid[0][0] == NULL)
	{
		printf ("OUT OF MEMORY\n");
		exit (1);
	}

	for (i = 1; i < xdim * ydim; i++)
		grid[0][i] = grid[0][i - 1] + zdim;



	return grid;
}

void free_3_double(int*** pppGrid)
{
	free(pppGrid[0][0]);
	free(pppGrid[0]);
	free(pppGrid);
}

void Generator::GenerateGrid(int ParticleCount, vector<CPnt>* pPoints, vector<CLink>* pLinks, vector<double> Dim)
{
	double PointDensity = ParticleCount / (Dim[0] * Dim[1] * Dim[2]);
	int Bin[3];
	Bin[0] = (int)Dim[0];
	Bin[1] = (int)Dim[1];
	Bin[2] = (int)Dim[2];

	int*** pppGrid = allocate_3d(Bin[0], Bin[1], Bin[2]);
	//int pppGrid[20][20][20];

	int Index = 0;

	for (int x = 0; x < Bin[0]; x++)
	{
		for (int y = 0; y < Bin[1]; y++)
		{
			for (int z = 0; z < Bin[2]; z++)
			{
				pppGrid[x][y][z] = genrand_real2() < PointDensity ? Index++ : -1;
			}
		}
	}

	for (int x = 0; x < Bin[0]; x++)
	{
		for (int y = 0; y < Bin[1]; y++)
		{
			for (int z = 0; z < Bin[2]; z++)
			{
				if (pppGrid[x][y][z] != -1)
				{
					if (pppGrid[(x + 1) % Bin[0]][y][z] != -1)
					{
						pLinks->push_back(CLink(pppGrid[x][y][z], pppGrid[(x + 1) % Bin[0]][y][z]));
					}

					if (pppGrid[x][(y + 1) % Bin[1]][z] != -1)
					{
						pLinks->push_back(CLink(pppGrid[x][y][z], pppGrid[x][(y + 1) % Bin[1]][z]));
					}

					if (pppGrid[x][y][(z + 1) % Bin[2]] != -1)
					{
						pLinks->push_back(CLink(pppGrid[x][y][z], pppGrid[x][y][(z + 1) % Bin[2]]));
					}

					pPoints->push_back(
						CPnt(
						static_cast<double>(x),
						static_cast<double>(y),
						static_cast<double>(z),
						pppGrid[x][y][z]));
				}
			}
		}
	}

	free_3_double(pppGrid);
}

void Generator::GeneratePoly(int ParticleCount, vector<CPnt>* pPoints, vector<CLink>* pLinks, vector<double> Dimensions)
{
	GeneratePoly(ParticleCount, pPoints, pLinks, Dimensions, 0.1);
}

void Generator::GeneratePoly(int ParticleCount, vector<CPnt>* pPoints, vector<CLink>* pLinks, vector<double> Dimensions, double ClickChance)
{		
	int PolySize = 20;
	double **pos;
	double dr[3];
	double bindim[3];
	int *list;
	int i,j,k,bx,by,bz,bdx,bdy,bdz,curprt,otherprt;
	int ***headlist, *linklist;

	//if((argc<6)||(argc>7))
	//{
	//	fprintf(stderr, "gennet basename xdim ydim zdim np\n");
	//	fprintf(stderr, "optional 6th argument for click chance\n");
	//	exit(EXIT_FAILURE);
	//}
	
	list= (int *)malloc(ParticleCount * sizeof(int));
	
	pos = allocate_2_double(ParticleCount, 3);

	//Generate random length polymers  
	for(i = 1; i < ParticleCount; i++)
	{
		if((i % PolySize) == 0)
		{
			list[i] = list[i - 1] + 1;
		}
		else
		{
			pLinks->push_back(CLink(i, i - 1));			
			list[i] = list[i-1];
		}
	}
	//randomly put in box
	pos[0][0] = genrand_real2() * Dimensions[0];
	pos[0][1] = genrand_real2() * Dimensions[1];
	pos[0][2] = genrand_real2() * Dimensions[2];

	for(i = 1; i < ParticleCount; i++)
	{
		if(list[i] == list[i-1])
		{
			pos[i][0] = pos[i-1][0] + gengauss(0.3,0);
			pos[i][1] = pos[i-1][1] + gengauss(0.3,0);
			pos[i][2] = pos[i-1][2] + gengauss(0.3,0);
		}
		else
		{
			pos[i][0] = genrand_real2() * Dimensions[0];
			pos[i][1] = genrand_real2() * Dimensions[1];
			pos[i][2] = genrand_real2() * Dimensions[2];
		}

		while(pos[i][0]<0)       pos[i][0]+=Dimensions[0];
		while(pos[i][0]>=Dimensions[0]) pos[i][0]-=Dimensions[0];
		while(pos[i][1]<0)       pos[i][1]+=Dimensions[1];
		while(pos[i][1]>=Dimensions[1]) pos[i][1]-=Dimensions[1];
		while(pos[i][2]<0)       pos[i][2]+=Dimensions[2];
		while(pos[i][2]>=Dimensions[2]) pos[i][2]-=Dimensions[2];
	}

	//fprintf(stderr, "Done with chains\r");


	//randomly introduce crosslinks

	//quick celllist construction for speedup
	//fprintf(stderr, "Cellist construction\r");

	headlist=allocate_3d((int)Dimensions[0], (int)Dimensions[1], (int)Dimensions[2]);

	linklist=(int *)malloc(sizeof(int) * ParticleCount);

	for(i = 0; i < (int)Dimensions[0]; i++)
		for(j = 0; j < (int)Dimensions[1]; j++)
			for(k = 0; k < (int)Dimensions[2]; k++)
				headlist[i][j][k] = -1;

	for(i=0;i<3;i++)
		bindim[i] = Dimensions[i] / (int)Dimensions[i];

	for(i = (ParticleCount-1); i >= 0; i--)
	{
		//fprintf(stderr, "Cellist construction %.1f%% done\r", 100.0*(double)(ParticleCount - i)/(double)ParticleCount);
		
		bx=(int)(pos[i][0]/bindim[0]);
		by=(int)(pos[i][1]/bindim[1]);
		bz=(int)(pos[i][2]/bindim[2]);

		if(headlist[bx][by][bz]==-1)
		{
			headlist[bx][by][bz]=i;
			linklist[i]=-1;
		}
		else
		{
			linklist[i]=headlist[bx][by][bz];
			headlist[bx][by][bz]=i;
		}
	}
	
	//fprintf(stderr, "                                            \r");

	for(bx=0;bx<(int)Dimensions[0];bx++)
	{
		for(by=0;by<(int)Dimensions[1];by++)
		{
			for(bz=0;bz<(int)Dimensions[2];bz++)
			{
				//fprintf(stderr, "Connections %.1lf%% done\r", 100.0*((bx*(int)Dimensions[1]*(int)Dimensions[2])+(by*(int)Dimensions[2])+(bz))/((int)Dimensions[0]*(int)Dimensions[1]*(int)Dimensions[2]));
				curprt=headlist[bx][by][bz];
				while(curprt!=-1)
				{
					for(bdx=-1;bdx<2;bdx++)
					{
						for(bdy=-1;bdy<2;bdy++)
						{
							for(bdz=-1;bdz<2;bdz++)
							{
								otherprt=headlist
									[((bx+bdx)+(int)Dimensions[0])%((int)Dimensions[0])]
								[((by+bdy)+(int)Dimensions[1])%((int)Dimensions[1])]
								[((bz+bdz)+(int)Dimensions[2])%((int)Dimensions[2])];
								while(otherprt!=-1)
								{
									if((list[curprt]!=list[otherprt])&&(genrand_real2() < ClickChance))
									{
										if(otherprt < curprt)
										{
											dr[0]=pos[curprt][0]-pos[otherprt][0];
											dr[1]=pos[curprt][1]-pos[otherprt][1];
											dr[2]=pos[curprt][2]-pos[otherprt][2];
											if(dr[0]>0.5*Dimensions[0])dr[0]-=Dimensions[0];
											if(dr[1]>0.5*Dimensions[1])dr[1]-=Dimensions[1];
											if(dr[2]>0.5*Dimensions[2])dr[2]-=Dimensions[2];
											if(dr[0]<(-0.5*Dimensions[0]))dr[0]+=Dimensions[0];
											if(dr[1]<(-0.5*Dimensions[1]))dr[1]+=Dimensions[1];
											if(dr[2]<(-0.5*Dimensions[2]))dr[2]+=Dimensions[2];
											if(sqrt((dr[0]*dr[0])+(dr[1]*dr[1])+(dr[2]*dr[2]))<1.0)
											{
												pLinks->push_back(CLink(curprt, otherprt));
											//	fprintf(fplink, "%d %d\n", curprt,otherprt);
											}
										}
									}
									otherprt=linklist[otherprt];
								}
							}
						}
					}
					curprt=linklist[curprt];
				}
			}
		}
	}
	//fprintf(stderr, "\rDone                                      \n");
	
	for(i = 0; i < ParticleCount; i++)
	{		
		pPoints->push_back(CPnt(pos[i][0], pos[i][1], pos[i][2], i));
	}

	free_3_double(headlist);
	free_2_double(pos);
	free(list);
	free(linklist);
}