#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

void push(vector<int> & last,vector<int> & next,int idx)
{
	int hdx = last.size()-1;
	int ndx = next[hdx];
	
	last[ndx] = idx;
	next[idx] = ndx;
	
	next[hdx] = idx;
	last[idx] = hdx;
}

void pop(vector<int> & last,vector<int> & next,int idx)
{
	int ndx = next[idx];
	int ldx = last[idx];
	
	next[ldx] = ndx;
	last[ndx] = ldx;

	next[idx] = last[idx] = -1;  
}

struct necklace_1d
{
	int N;
	vector<int> last;
	vector<int> next;
	vector<int> cont;
	necklace_1d(int N_)
	{
		N = N_;
		last = vector<int>(N+1,-1);
		next = vector<int>(N+1,-1);
		last[N] = next[N] = N;
		
		cont = vector<int>(N,0);
	}
	~necklace_1d()
	{
		cont.clear();
		last.clear();
		next.clear();
	}
	void add(int idx,int amt,bool era=true)
	{
		cont[idx]+=amt;
		if(last[idx] == -1 && next[idx] == -1)
			push(last,next,idx);
		if(era && cont[idx] == 0)
			pop(last,next,idx);
	}
};

struct necklace_2d
{
	int N;
	int M;
	vector<necklace_1d*> col;
	
	necklace_2d(int N_,int M_)
	{
		N = N_;
		M = M_;
		col = vector<necklace_1d*>(M_);
		for(int m=0;m<M;m++)
			col[m] = new necklace_1d(N_);
	}
	~necklace_2d()
	{
		for(int m=0;m<M;m++)
			delete col[m];
		col.clear();
	}
	
	void add(int idx,int jdx,int amt,bool era=true)
	{
		col[jdx]->add(idx,amt,era);
	}
};

void shift_left(necklace_2d & H, necklace_1d & B,int & c)
{
	int c_last = (c-1+H.M)%H.M;	
	necklace_1d * aux = H.col[c];
	for(int y=aux->next[aux->N];y!=aux->N;y=aux->next[y])
	{
		B.add(y,-2*aux->cont[y]);
	}
	c = c_last;
}

void shift_right(necklace_2d & H, necklace_1d & B,int & c)
{
	int c_next = (c+1+H.M)%H.M;
	necklace_1d * aux = H.col[c_next];
	for(int y = aux->next[aux->N];y!=aux->N;y = aux->next[y])
	{
		B.add(y,2*aux->cont[y]);
	}
	c = c_next;
}

double balance_bcb(vector<vector<double> > & distY,necklace_1d & B,int yp)
{
	double b = 0.0;
	for(int y = B.next[B.N];y!=B.N;y = B.next[y])
	{
		b += distY[y][yp]*B.cont[y];
	}
	return b;
}

/*
 * X & Y are the indexes of the clustered spaces, X could be color, Y the features.
 * distY are the pairwise distances of the clustered features.
 * 
 * H is the joint histogram
 * B is the balance counting box
*/

vector<vector<int> > fwmf_cpp(vector<vector<int> > & X,vector<vector<int> > & Y, vector<vector<double> > & distY,int nX,int nY,int R)
{
	int N =    X.size();
	int M = X[0].size();
	
	vector<vector<int> > C(N,vector<int>(M,0));

	necklace_2d H(nX,nY);
	necklace_1d B(nY);
	
	for(int nn=-R;nn<=R;nn++)
	for(int mm=-R;mm<=R;mm++)
		if(0<=nn&&nn<N&&0<=mm&&mm<M)
		{
			int x = X[nn][mm];
			int y = Y[nn][mm];
			H.add(x,y,1);
			B.add(y,1);
		}
	
	
	C[0][0] = 0;
	
	for(int n=0;n<N;n++)
	{
		int  step_m = n%2?  -1:   1;
		int       m = n%2? M-1:   0;
		
		while(0<=m+step_m&&m+step_m<=M-1)
		{
			//Shift cutpoint for p=(n,m)
			double best_Bc = balance_bcb(distY,B,Y[n][m]);
			if(best_Bc > 0)
			{
				while(best_Bc > 0)
				{
					if(C[n][m] == 0) break;

					shift_left(H,B,C[n][m]);
					best_Bc = balance_bcb(distY,B,Y[n][m]);
				}
				//shift_right(H,B,C[n][m]);
				//best_Bc = balance_bcb(distY,B,Y[n][m]);
			}
			else if(best_Bc < 0)
			{
				while(best_Bc < 0)
				{
					if(C[n][m] == nX-1) break;

					shift_right(H,B,C[n][m]);
					best_Bc = balance_bcb(distY,B,Y[n][m]);
				}
				//shift_left(H,B,C[n][m]);
				//best_Bc = balance_bcb(distY,B,Y[n][m]);			
			}
			//Actualize Histogram L/R or R/L
			//Median for next window is median of the last... and will be shifted if necessary
			C[n][m+step_m] = C[n][m];
			for(int nn=n-R;nn<=n+R;nn++)
			if(0<=nn&&nn<N)
			{
				int mm_last = m-step_m*R;
				int mm_next = m+step_m*(R+1);
				if(0<=mm_last&&mm_last<M)
				{
					int xo = X[nn][mm_last];
					int yo = Y[nn][mm_last];
					H.add(xo,yo,-1);
					B.add(yo,xo>C[n][m]?1:-1);
				}
				if(0<=mm_next&&mm_next<M)
				{
					int xn = X[nn][mm_next];
					int yn = Y[nn][mm_next];
					H.add(xn,yn,1);
					B.add(yn,xn<=C[n][m]?1:-1);
				}
			}
			m += step_m;
		}
		//Actualize Histogram U/D
		if(n<N-1)
		{
			//Median for next window is median of the last... and will be shifted if necessary
			C[n+1][m] = C[n][m];
			for(int mm=m-R;mm<=m+R;mm++)
			if(0<=mm&&mm<M)
			{
				int nn_last = n-R;
				int nn_next = n+R+1;
				if(0<=nn_last)
				{
					int xo = X[nn_last][mm];
					int yo = Y[nn_last][mm];
					H.add(xo,yo,-1);
					B.add(yo,xo>C[n][m]?1:-1);
				}
				if(nn_next<N)
				{
					int xn = X[nn_next][mm];
					int yn = Y[nn_next][mm];
					H.add(xn,yn,1);
					B.add(yn,xn<=C[n][m]?1:-1);
				}
			}
		}
	}
	return C;
}
