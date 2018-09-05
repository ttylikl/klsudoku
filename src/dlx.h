#ifndef __INCLUDE_DLX_H__
#define __INCLUDE_DLX_H__
#include <string>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

using namespace std;

typedef struct node
{
	int r,c;
	node *up;
	node *down;
	node *left;
	node *right;
}node;

class DLXSolver
{
public:
	DLXSolver():
		chk_unique(0)
	{
		all=(node*)malloc(RR*CC+99);
	}
	~DLXSolver()
	{
		free(all);
	}
private:
	int chk_unique;
	enum {RR=729,CC=324,INF=1000000000};
	int mem[RR+9];
	int ans[RR+9];
	char ch[RR+9];
	int cnt[CC+9];
	node *all;
	node head/*,all[RR*CC+99]*/,row[RR],col[CC];
	int all_t;
	void link(int r,int c)
	{
		cnt[c]++;
		node *t=&all[all_t++];
		t->r=r;
		t->c=c;
		t->left=&row[r];
		t->right=row[r].right;
		t->left->right=t;
		t->right->left=t;
		t->up=&col[c];
		t->down=col[c].down;
		t->up->down=t;
		t->down->up=t;
	}
	void remove(int c)
	{
		node *t,*tt;
		col[c].right->left=col[c].left;
		col[c].left->right=col[c].right;
		for(t=col[c].down;t!=&col[c];t=t->down)
		{
			for(tt=t->right;tt!=t;tt=tt->right)
			{
				cnt[tt->c]--;
				tt->up->down=tt->down;
				tt->down->up=tt->up;
			}
			t->left->right=t->right;
			t->right->left=t->left;
		}
	}
	void resume(int c)
	{
		node *t,*tt;
		for(t=col[c].down;t!=&col[c];t=t->down)
		{        
			t->right->left=t;
			t->left->right=t;
			for(tt=t->left;tt!=t;tt=tt->left)
			{
				cnt[tt->c]++;
				tt->down->up=tt;
				tt->up->down=tt;
			}
		}    
		col[c].left->right=&col[c];
		col[c].right->left=&col[c];
	}
	int solve(int k)
	{
		if(head.right==&head)
			return true;
		node*t,*tt;
		int min=INF,tc=0;
		for(t=head.right;t!=&head;t=t->right)
		{
			if(cnt[t->c]<min)
			{
				min=cnt[t->c];
				tc=t->c;
				if(min<=1)break;
			}
		}
		remove(tc);
		int scnt=0;
		for(t=col[tc].down;t!=&col[tc];t=t->down)
		{
			mem[k]=t->r;
			t->left->right=t;
			for(tt=t->right;tt!=t;tt=tt->right)
			{
				remove(tt->c);
			}
			t->left->right=t->right;
			//if(solve(k+1))
			//	return true;
			scnt+=solve(k+1);
			if(!chk_unique && scnt==1)
				return scnt;
			if(scnt>1)
				return scnt;
			t->right->left=t;
			for(tt=t->left;tt!=t;tt=tt->left)
			{
				resume(tt->c);
			}
			t->right->left=t->left;
		}
		resume(tc);
		return scnt;
	}
public:
	//This is not a threadsafe class! so do not access same instance within more than 1 thread
	int solution_count(const char *psz)
	{
		chk_unique=1; //set to check unique mode
		int scnt=0;
		solution(psz,&scnt);
		chk_unique=0; //recover to solution mode
		return scnt;
	}
	//This is not a threadsafe class! so do not access same instance within more than 1 thread
	string solution(const char *psz,int *scnt)
	{
		if(strlen(psz)<81)
		{
			if(scnt) *scnt=-1;
			return "";
		}
		// double ss=0;

		memset(all,0,RR*CC+99);
		memset(mem,0,sizeof(mem));
		memset(ans,0,sizeof(ans));
		memset(cnt,0,sizeof(cnt));
		strcpy(ch,psz);

		{
			int i;//,j,k;
			//if(ch[0]=='e')break;
			all_t=1;
			memset(cnt,0,sizeof(cnt));
			head.left=&head;
			head.right=&head;
			head.up=&head;
			head.down=&head;
			head.r=RR;
			head.c=CC;
			for(i=0;i<CC;i++)
			{
				col[i].c=i;
				col[i].r=RR;
				col[i].up=&col[i];
				col[i].down=&col[i];
				col[i].left=&head;
				col[i].right=head.right;
				col[i].left->right=&col[i];
				col[i].right->left=&col[i];
			}
			for(i=0;i<RR;i++)
			{
				row[i].r=i;
				row[i].c=CC;
				row[i].left=&row[i];
				row[i].right=&row[i];
				row[i].up=&head;
				row[i].down=head.down;
				row[i].up->down=&row[i];
				row[i].down->up=&row[i];
			}
			for(i=0;i<RR;i++)
			{
				int r=i/9/9%9;
				int c=i/9%9;
				int val=i%9+1;
				if(ch[r*9+c]=='.'||ch[r*9+c]=='0'||ch[r*9+c]==val+'0')
				{
					link(i,r*9+val-1);
					link(i,81+c*9+val-1);
					int tr=r/3;
					int tc=c/3;
					link(i,162+(tr*3+tc)*9+val-1);
					link(i,243+r*9+c);
				}
			}
			for(i=0;i<RR;i++)
			{
				row[i].left->right=row[i].right;
				row[i].right->left=row[i].left;
			}
			int sc=solve(1);
			if(sc!=1)
			{
				if(scnt) *scnt=-2;
				return "";
			}
			for(i=1;i<=81;i++)
			{
				int t=mem[i]/9%81;
				int val=mem[i]%9+1;
				ans[t]=val;
			}
			char ret[90]={0};
			for(i=0;i<81;i++)
			{
				//printf("%d",ans[i]);
				ret[i]='0'+ans[i];
			}
			if(scnt) *scnt=sc;
			return ret;
		}
	}

	string generate()
	{
		char res[82];
		memset(res,'.',81);
		res[81] = 0;
		//step1 get one solution
		struct timeval tv;
    gettimeofday(&tv,NULL);
    // printf("second:%ld\n",tv.tv_sec);  //秒
    // printf("millisecond:%ld\n",tv.tv_sec*1000 + tv.tv_usec/1000);  //毫秒
    // printf("microsecond:%ld\n",tv.tv_sec*1000000 + tv.tv_usec);  //微秒
		// unsigned int seed = (unsigned int)time(0);
		unsigned int seed = tv.tv_usec;
		srand(seed);
		// printf ("rand seed: %d\n", seed);
		for(int i=0;i<9;i++)
		{
			res[i]='1'+i;
		}
		for(int i=0;i<9;i++)
		{
			int j=rand()%81;
			if(j!=i)
			{
				char ch=res[j];
				res[j]=res[i];
				res[i]=ch;
			}
		}
		// printf("res: %s\n", res);

		string str=solution(res,0);
		strcpy(res,str.c_str());
		//step2 prepare random remove order
		int rid[81];
		for(int i=0;i<81;i++)
			rid[i]=i;
		for(int i=0;i<81;i++)
		{
			int j=rand()%81;
			if(j!=i)
			{
				int n=rid[i];
				rid[i]=rid[j];
				rid[j]=n;
			}
		}
		//step3 try to remove number & verify it
		for(int i=0;i<81;i++)
		{
			char ch=res[rid[i]];
			res[rid[i]]='.';
			if(solution_count(res)!=1)
			{
				res[rid[i]]=ch;
			}
		}
		//step4 finally, we got a new unique & we can't remove any number from it. If we do it, the puzzle won't be unique!
		// printf("return %s\n", res);
		return res;
	}
};
#endif //__INCLUDE_DLX_H__
