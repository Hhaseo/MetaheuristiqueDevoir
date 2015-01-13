#include <stdio.h>
#include <math.h>
#include <stdlib.h>



/* function declarations */
float ran2(long *idum);
void StartCSP(int N, int K, int instance);
void EndCSP();
void AddConstraint(int var1, int var2);
void AddNogood(int val1, int val2);
int MakeURBCSP(int N, int D, int C, int T, long *Seed,int *constraint1[],int **constraint2[]);




int evolution_urbcsp2(int N, int  D, int  C, int  T, long S, int I,int **constraint1,int ***constraint2)
{
  int i;
  
  if (S > 0)
    S = -S;

  for (i=0; i<I; ++i)
    if (!MakeURBCSP(N, D, C, T, &S,constraint1,constraint2))
      return 0;

  return 1;
}

int MakeURBCSP(int N, int D, int C, int T, long *Seed,int *constraint1[],int **constraint2[])
{
  
  
  int PossibleCTs, PossibleNGs;       /* CT means "constraint" */
  unsigned long *CTarray, *NGarray;   /* NG means "nogood pair" */
  long selectedCT, selectedNG;
  int i, c, r, t;
  int var1, var2;
  static int instance;

  /* Check for valid values of N, D, C, and T. */
  if (N < 2)
    {
      printf("MakeURBCSP: ***Illegal value for N: %d\n", N);
      return 0;
    }
  if (D < 2)
    {
      printf("MakeURBCSP: ***Illegal value for D: %d\n", D);
      return 0;
    }
  if (C < 0 || C > N * (N - 1) / 2)
    {
      printf("MakeURBCSP: ***Illegal value for C: %d\n", C);
      return 0;
    }
  if (T < 1 || T > ((D * D) - 1))
    {
      printf("MakeURBCSP: ***Illegal value for T: %d\n", T);
      return 0;
    }

  if (*Seed < 0)      /* starting a new sequence of random numbers */
    instance = 0;
  else
    ++instance;       /* increment static variable */

  StartCSP(N, D, instance);
  
  
  PossibleCTs = N * (N - 1) / 2;
  CTarray = (unsigned long*) malloc(PossibleCTs * 4);

  /* Create an array for each possible value pair. */
  PossibleNGs = D * D;
  NGarray = (unsigned long*) malloc(PossibleNGs * 4);

 
  i=0;
  for (var1=0; var1<(N-1); ++var1)
    for (var2=var1+1; var2<N; ++var2)
      CTarray[i++] = (var1 << 16) | var2;

  /* Select C constraints. */
  for (c=0; c<C; ++c)
    {
      int n,m;
      /* Choose a random number between c and PossibleCTs - 1, inclusive. */
      r =  c + (int) (ran2(Seed) * (PossibleCTs - c)); 

      /* Swap elements [c] and [r]. */
      selectedCT = CTarray[r];
      CTarray[r] = CTarray[c];
      CTarray[c] = selectedCT;
      m=(int)(CTarray[c] & 0x0000FFFF);
      n=(int)(CTarray[c] >> 16);
       constraint1[n][m]=c+1;
       
      
       AddConstraint(n,m);

     
      for (i=0; i<(D*D); ++i)
        NGarray[i] = i;

      /* Select T incompatible pairs. */
      for (t=0; t<T; ++t)
        {
          /* Choose a random number between t and PossibleNGs - 1, inclusive.*/
	  int a,b;         
	  r =  t + (int) (ran2(Seed) * (PossibleNGs - t));
          selectedNG = NGarray[r];
          NGarray[r] = NGarray[t];
          NGarray[t] = selectedNG;
	  a=(int)(NGarray[t] / D);
	  b=(int)(NGarray[t] % D);
        
	  constraint2[c][a][b]=1;
	 
	  AddNogood(a,b);
        }
    }
    
  EndCSP();
  free(CTarray);
  free(NGarray);

  return 1;
}





#define IM1   2147483563
#define IM2   2147483399
#define AM    (1.0/IM1)
#define IMM1  (IM1-1)
#define IA1   40014
#define IA2   40692
#define IQ1   53668
#define IQ2   52774
#define IR1   12211
#define IR2   3791
#define NTAB  32
#define NDIV  (1+IMM1/NTAB)
#define EPS   1.2e-7
#define RNMX  (1.0 - EPS)



float ran2(long *idum)
{
  int j;
  long k;
  static long idum2 = 123456789;
  static long iy = 0;
  static long iv[NTAB];
  float temp;

  if (*idum <= 0) {                             /* initialize */
    if (-(*idum) < 1)                           /* prevent idum == 0 */
      *idum = 1;
    else
      *idum = -(*idum);                         /* make idum positive */
    idum2 = (*idum);
    for (j = NTAB + 7; j >= 0; j--) {           /* load the shuffle table */
      k = (*idum) / IQ1;
      *idum = IA1 * (*idum - k*IQ1) - k*IR1;
      if (*idum < 0)
        *idum += IM1;
      if (j < NTAB)
        iv[j] = *idum;
    }
    iy = iv[0];
  }
      
  k = (*idum) / IQ1;
  *idum = IA1 * (*idum - k*IQ1) - k*IR1;
  if (*idum < 0)
    *idum += IM1;
  k = idum2/IQ2;
  idum2 = IA2 * (idum2 - k*IQ2) - k*IR2;
  if (idum2 < 0)
    idum2 += IM2;
  j = iy / NDIV;
  iy = iv[j] - idum2;
  iv[j] = *idum;
  if (iy < 1)
    iy += IMM1;
  if ((temp = AM * iy) > RNMX)
    return RNMX;                                /* avoid endpoint */
  else
    return temp;
}



void StartCSP(int N, int D, int instance)
{
//  printf("\nInstance %d", instance);
}

void AddConstraint(int var1, int var2)
{
  // printf("\n%3d %3d: ", var1, var2);
}

void AddNogood(int val1, int val2)
{
  //  printf("(%d %d) ", val1, val2);
}

void EndCSP()
{
  //  printf("\n");
}
