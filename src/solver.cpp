#include "common.h"
#include "objects.h"
#include "funcs_temp.h"

#ifdef _WIN32
#define PRPR(initer) DWORD st = GetTickCount();\
			DWORD nt;\
			DWORD lt = st;\
			nt = GetTickCount();\
			plog("%s: %d - %d", #initer, nt - lt, nt - st);\
			lt = nt;
#else
#define PRPR(initer)
#endif

#ifdef _WIN32
#define PRST( helper ) { nt=GetTickCount(); if(nt-lt) { plog("%s: %d - %d ", #helper, nt - lt, nt - st );  lt=nt; } }
#else
#define PRST( helper) {plog("%s", #helper);}
#endif

void testvv()
{
	vector<int> v1;
	vector<int> v2;
	v1.push_back(1);
	v1.push_back(3);
	v1.push_back(1);
	v1.push_back(4);

	v2.push_back(1);
	v2.push_back(2);
	v2.push_back(2);
	v2.push_back(5);

	vector<int> v3(v2);
	vv_equal(v1,v2);
	vv_equal(v2,v3);
	vv_contain(v3,v1);
	vv_contain(v3,v2);
	v3.push_back(6);

	vv_equal(v3,v2);
	vv_contain(v3,v2);
	vv_contain(v2,v3);


	//dumpstr(v1);
	//dumpstr(v2);

	//vector<int> v3=vv_union(v1,v2);
	//dumpstr(v3);
	//v3=vv_intersection(v1,v2);
	//dumpstr(v3);
	//v3=vv_difference(v1,v2);
	//dumpstr(v3);
	//v3=vv_difference(v2,v1);
	//dumpstr(v3);
}

vector<vector<vector<int> > > Solver::vvv;

Solver::~Solver()
{
	m_curNum = 1;
}

Solver::Solver()
{
	//memset(_nodes,0,sizeof(_nodes)); //Bad Code!!!
	//testvv();
	if(vvv.size()==0)
	{
		vector<vector<int> > vp;
		vector<vector<int> > vt;
		vector<vector<int> > vq;
		for(int i=0;i<NUM;++i)
		{
			for(int j=i+1;j<NUM;++j)
			{
				vector<int> vl;
				vl.push_back(i);
				vl.push_back(j);
				vp.push_back(vl);
				for(int k=j+1;k<NUM;++k)
				{
					vector<int> vl;
					vl.push_back(i);
					vl.push_back(j);
					vl.push_back(k);
					vt.push_back(vl);
					for(int l=k+1;l<NUM;++l)	
					{	
						vector<int> vl;
						vl.push_back(i);
						vl.push_back(j);
						vl.push_back(k);
						vl.push_back(l);
						vq.push_back(vl);
					}
				}
			}
		}
		vvv.push_back(vp);
		vvv.push_back(vt);
		vvv.push_back(vq);
#if !defined(ANDROID) && (defined(DEBUG) || defined(_DEBUG))
		char buf[20480]="";
		for(unsigned int i=0;i<vvv.size();++i)
		{
			vector<vector<int> > vv=vvv[i];
			strcat(buf,"{");
			for(unsigned int j=0;j<vv.size();++j)
			{
				strcat(buf,"{");
				vector<int> v=vv[j];
				for(unsigned int k=0;k<v.size();++k)
				{
					char ns[]={0,',',0};
					ns[0]='0'+v[k];
					strcat(buf,ns);
				}
				strcat(buf,"},");
			}
			strcat(buf,"}\n");
			plog(buf);
			*buf=0;
		}
#endif
	}
	m_curNum = 1; // 缺省值
};

const char *Solver::getSolverName(EnumSolver solver)
{
	return toSolverName(solver);
}

int Solver::executeSActions(Puzzle &pz,vector<SAction> &acts)
{
	for(unsigned int i=0;i<acts.size();i++)
	{
		SAction *pAct = &acts[i];
		for(unsigned int opi=0;opi<pAct->ops.size();++opi)
		{
			Operation op=pAct->ops[opi];
			for(unsigned int pi=0;pi<op.pts.size();++pi)
			{
				Cell *cell=pz.getCell(op.pts[pi]);
				if(op.tp==ACT_SET)
				{
					//if (cell->getValue()!=NUM_NONE) plog("ERROR");
					cell->setValue(op.nums[0]);
				}
				else if(op.tp==ACT_TAG)
				{
					for(unsigned int ni=0;ni<op.nums.size();++ni)
					{
						int n=op.nums[ni];
						cell->setCandStatus(n,ST_TAG);
					}
				}
				else plog("ERROR");
			}
		}
	}
	return 0;
}

vector<SAction> Solver::doSolve(Puzzle *pz,int hint)
{
	srand((int)time(0));
	_pz.copyFrom(*pz);
	_acts.clear();
	DLXSolver dlx;
	vector<SAction> acts;
	string spz=_pz.dumps(FMT_LINE);
	//if(dlx.solution_count(_pz.dumps(FMT_LINE))!=1) return _acts;
	int scnt=0;
	_szResult=dlx.solution(spz.c_str(),&scnt);
	if(scnt<1)
	{
		plog("Puzzle has wrong fill, can't solve it!");
	}
	int loop=0;
	resetNodes();
	updateNodes();
	while(true)
	{
		loop++;
		if(hint==2 && loop > 5) //if we loop more than 5, and can't get ACT_SET SAction, we still break loop.
			break;

		//If Puzzle is done, break loop.
		if(_pz.isDone()) //
			break;
	
		PRPR("doSolve");

		//Find SActions
		int bAct=1;
		do 
		{
			if(trySingleNumber(_pz, acts)>0)
			{
				bAct=1;
				break;
			}
			bAct=0; 
			//The trying below can't get ACT_SET, we should loop to try!


			if(tryIntersectionRemoval(_pz,acts) >0)
				break;
			PRST(tryIntersectionRemoval);
			if(tryHiddenNumberSet(_pz,acts,2)>0)
				break;
			PRST(tryHiddenNumberSet2);
			if(tryHiddenNumberSet(_pz,acts,3)>0)
				break;
			PRST(tryHiddenNumberSet3);
			if(tryHiddenNumberSet(_pz,acts,4)>0)
				break;
			PRST(tryHiddenNumberSet4);

			if(tryNakedNumberSet(_pz,acts,2)>0)
				break;
			PRST(tryNakedNumberSet2);
			if(tryNakedNumberSet(_pz,acts,3)>0)
				break;
			PRST(tryNakedNumberSet3);
			if(tryNakedNumberSet(_pz,acts,4)>0)
				break;
			PRST(tryNakedNumberSet4);

			if(tryFish(_pz,acts,2)>0) //X-Wing, SwordFish, JellyFish
				break;
			PRST(tryFish2);
			if(tryFish(_pz,acts,3)>0) //X-Wing, SwordFish, JellyFish
				break;
			PRST(tryFish3);
			if(tryFish(_pz,acts,4)>0) //X-Wing, SwordFish, JellyFish
				break;
			PRST(tryFish4);

			if(tryXYWing(_pz,acts)>0) //XY-Wing(Y-Wing)
				break;
			PRST(tryXYWing);
			if(tryXYZWing(_pz,acts)>0) //XYZ-Wing
				break;
			PRST(tryXYZWing);
			if(tryUniqueRectangle(_pz,acts)>0) //XY-Wing(Y-Wing)
				break;
			PRST(tryUniqueRectangle);
			if(tryBug1(_pz,acts)>0)
			{
				bAct=1;
				break;
			}
			PRST(tryBug1);
			if(tryXChain(_pz,acts)>0)
				break;
			PRST(tryXChain);
			if(tryXYChain(_pz,acts)>0)
				break;
			PRST(tryXYChain);
			if(tryForcingChain(_pz,acts)>0)
				break;
			PRST(tryForcingChain);
			if(tryGuess(_pz,acts)>0) //Make sure we always can have a hint !
			{
				bAct=1;
				break;
			}
			else
			{
				// Puzzle has wrong fill, guess is useless. so we do nothing here.
				return acts;
			}
		} while (false);
		
		PRST(last_do_while);

		if(hint==1)
			return acts;//for hint once
		//We don't have any ACT_SET!
		if(bAct==0)
		{
			plog("TODO:We haven't found ACT_SET, we have to Execute these SActions, then enter next loop!");
			//Execute SActions
			executeSActions(_pz,acts);
			//Save SActions
			_acts.insert(_acts.end(),acts.begin(),acts.end());
			acts.clear();
			continue;
		}
		if(hint) // && bAct!=0 ...
			break; // We have got one tip SAction, break loop and return it.
	}

	//Save SActions
	_acts.insert(_acts.end(),acts.begin(),acts.end());

	return _acts;
}

int *Solver::randIDX()
{
	static int idx[NUMCOL*NUMROW];
	int i;
	for(i=0;i<NUMCOL*NUMROW;i++)
	{
		idx[i]=i;
	}
	for(i=0;i<NUMCOL*NUMROW;i++)
	{
		int j=rand()%NUMCOL*NUMROW;
		if(i!=j)
		{
			int n=idx[i];
			idx[i]=idx[j];
			idx[j]=n;
		}
	}

	//code for debug
#ifndef NDEBUG
	char msgbuf[NUMCOL*NUMROW*4];
	*msgbuf=0;
	for(i=0;i<NUMCOL*NUMROW;i++)
	{
		char tmp[10];
		sprintf(tmp,"%2d,",idx[i]);
		strcat(msgbuf,tmp);
	}
	plog("randIDX:%s",msgbuf);
#endif
	return idx;
}

int Solver::tryGuess(Puzzle &pz,vector<SAction>&acts)
{
	int *idx=randIDX();
	vector<Cell*> cells=pz.getCells();
	vector<int> cands;
	Cell *cell=NULL;
	Puzzle pzResult;
	pzResult.loads(_szResult.c_str());
	for(int i=0;i<NUMCOL*NUMROW;i++)
	{
		Cell *c=cells[idx[i]];
		if(c->getValue()!=NUM_NONE)
			continue;
		cands=c->getCands(ST_NORMAL);
		cell=c;
		int n=cands.size();
		if(n<3)
		{
			SPoint pt=cell->getPoint();
			int num=pzResult.getCell(pt.getx(),pt.gety())->getValue();
			if(num==NUM_NONE)
			{
				plog("Wrong Puzzle, we do not guess! ");
				return 0;
			}
			SAction act(S_Guess,Operation(ACT_SET,cell->getPoint(),num));//,ACT_SET,cell->getPoint(),num,ST_NONE);
			acts.push_back(act);
			return 1;
		}
	}
	if(cands.size()>0 && cell!=0)
	{
		SPoint pt=cell->getPoint();
		int num=pzResult.getCell(pt.getx(),pt.gety())->getValue();
		if(num==NUM_NONE)
		{
			plog("Wrong Puzzle, we do not guess! ");
			return 0;
		}
		SAction act(S_Guess,Operation(ACT_SET,cell->getPoint(),num,ST_NONE));
		acts.push_back(act);
		return 1;
	}
	return 0;
}

int Solver::trySingleNumber(Puzzle &pz,vector<SAction> &acts)
{
	//NakedSingleNumber & HiddenSingleNumber
	int ret=0;
	vector<Cell*> cells;

	vector<CellSet *> css = pz.getCellSetList();
	int num = m_curNum;
	while (true)
	{
		//for (int num = 1; num <= NUM; num++) //always start lookup from 1, but we hope player focus the current number
		for (unsigned int i = 0; i < css.size(); ++i)
		{
			cells = css[i]->getCells();
			{
				int sum = 0;
				Cell *cell = NULL;
				for (unsigned int j = 0; j < cells.size(); ++j)
				{
					Cell *c = cells[j];
					if (c->getValue() != NUM_NONE) continue;
					if (c->getCandStatus(num) == ST_NORMAL)
					{
						sum++;
						cell = c;
					}
				}
				if (sum == 1)
				{
					if (cell->getCands(ST_NORMAL).size() > 1)
					{
						//Found a hidden single number
						//css[i]->getType();
						SPoint pt = cell->getPoint();
						SAction act(S_HiddenSingleNumber);//,ACT_SET,cell->getPoint(),num,ST_NONE);
						act.cellsets.push_back(css[i]);
						Operation op(ACT_SET);
						op.pts.push_back(cell->getPoint());
						op.nums.push_back(num);
						act.ops.push_back(op);
						acts.push_back(act);
						ret++;
						return ret;
					}
				}
			}
		}
		//try next num

		num = (num + 1) % (NUM + 1);
		if (num == 0) num++;
		if (num == m_curNum) break;
	}
	cells=pz.getCells();
	for(unsigned int i=0;i<cells.size();++i)
	{
		Cell* cell=cells[i];
		if(cell->getValue()!=NUM_NONE) continue;
		vector<int> cands=cell->getCands(ST_NORMAL);
		if(cands.size()==1)
		{
			SPoint pt=cell->getPoint();
			int num=cands[0];
			SAction act(S_NakedSingleNumber);//ACT_SET,cell->getPoint(),num,ST_NONE);
			//TODO: Difficult level thought: If one of CellSets which connect to cell has more than 4 Numbers filled, we should use insert to begin(), not push_back()!
			Operation op(ACT_SET);
			op.pts.push_back(cell->getPoint());
			op.nums.push_back(num);
			act.ops.push_back(op);
			acts.push_back(act);
			ret++;
			return ret;
		}
	}
	return ret;
}

int Solver::tryIntersectionRemoval(Puzzle &pz,vector<SAction> &acts)
{
	int ret=0;
	vector<CellSet*> cellsets=pz.getCellSetList();
	//plog("CellSets size is :%d!", cellsets.size());
	for(unsigned int csi=0;csi<cellsets.size();++csi)
	{
		CellSet *cs=cellsets[csi];
		//plog("Scanning CellSet: %s" , cs->getName());
		vector<Cell *> cells=cs->getCells();

		int xybc[3][NUM];
		memset(xybc,0,sizeof(xybc));

		int cstype=cs->getType();
		for(unsigned int ci=0;ci<cells.size();++ci)
		{
			Cell *cell=cells[ci];
			if(NUM_NONE!=cell->getValue()) continue;

			vector<int> cands=cell->getCands(ST_NORMAL);
			SPoint pt=cell->getPoint();
			int x=pt.getx()+1; 
			int y=pt.gety()+1; 
			int b=cord2boxid(x-1,y-1)+1;
			int v[3]={x,y,b}; 
			//v[ROW]=x;v[COL]=y;v[BOX]=b;
			//plog("process cell[%s]",cell->getName());
			for(unsigned int ni=0;ni<cands.size();++ni)
			{
				int n=cands[ni];
				for(int xyb=0;xyb<3;xyb++) //xyb is ROW=0,COL=1,BOX=2
				{
					if( (cstype==BOX && xyb!=BOX) || cstype!=BOX && xyb==BOX)
					{
						if(xybc[xyb][n-1]!=-1)
						{
							if(xybc[xyb][n-1]==0)
							{
								//plog("Cand[%d] @ %s, set xc[%d]=%d",  n ,pt.getName(), n-1, x);
								xybc[xyb][n-1]=v[xyb];
							}
							else if(xybc[xyb][n-1]!=v[xyb])
								xybc[xyb][n-1]=-1;

						}
					}
				}
			}
		}
		int csid=cs->getID();
		const char *sz[]={"ROW","COL","BOX"};
		//sz[ROW]="ROW";sz[COL]="COL";sz[BOX]="BOX";
		for(int xyb=0;xyb<3;xyb++) //xyb is ROW=0,COL=1,BOX=2
		{
			for(int n=0;n<NUM;++n)
			{
				int cand=n+1;
				int RC;
				const char *szRC;
				if( (cstype==BOX && xyb!=BOX) || (cstype!=BOX && xyb==BOX))
				{
					if(xybc[xyb][n]>0){
						RC=xybc[xyb][n]; szRC=sz[xyb];
					}
					else continue;//skip for cand
				}
				else continue;

				CellSet *tcs=pz.getCellSet(xyb,RC-1);
				//plog("Found intersection:cand[%d] %s%d vs %s ,Check target CellSet:%s!", cand,  szRC, RC, cs->getName(), tcs->getName() );
				vector<Cell *> tcells=tcs->getCells();
				vector<SPoint> result;
				for(unsigned int i=0;i<tcells.size();++i)
				{
					Cell *tc=tcells[i];
					if(NUM_NONE!=tc->getValue())
						continue;
					SPoint pt=tc->getPoint();
					int tid=(cstype==BOX)?cord2boxid(pt.getx(),pt.gety()):(cstype==ROW?pt.getx():pt.gety());
					//plog("check %s tid=%d vs csid=%d",pt.getName(),tid,csid);
					if(tid==csid)
						continue;
					if(tc->getCandStatus(cand)==ST_NORMAL)
					{
						result.push_back(pt);
					}
				}
				if(result.size()>0)
				{
#if defined(DEBUG) || defined(_DEBUG)
					char szpt[1024]="";
					for(unsigned int i=0;i<result.size();++i)
					{
						strcat(szpt,"<");
						strcat(szpt,result[i].getName());
						strcat(szpt,">");
					}
					plog("Found SAction: Because %s , we remove cand[%d] from %s in %s", cs->getName(), cand,  szpt, tcs->getName() );
#endif
					SAction act(S_IntersectionRemoval);//,ACT_TAG,result[0],cand,ST_TAG);
					//do not modify the order of push_back() ! 
					act.cellsets.push_back(cs);
					act.cellsets.push_back(tcs);
					Operation op(ACT_TAG);
					op.nums.push_back(cand);
					op.pts=result;
					act.ops.push_back(op);
					acts.push_back(act);
					ret++;
					return ret;
				}
			}
		}
	}
	return ret;
}

int Solver::tryBug1(Puzzle &pz,vector<SAction> &acts)
{
	vector<Cell *> cells=pz.getCells();
	Cell *tc=NULL;
	for(unsigned int ci=0;ci<cells.size();++ci)
	{
		Cell *cell=cells[ci];
		if(cell->getValue()!=NUM_NONE) continue;
		vector<int> cands=cell->getCands(ST_NORMAL);
		if(cands.size()>3)
		{
			return 0; //In bug1, no cell has more than 3 cands
		}
		if(cands.size()==3)
		{
			if(tc!=NULL)
			{
				return 0;// more than 1 cell has 3 cands
			}
			tc=cell;
		}
	}
	if(!tc)
	{
		plog("All cells has 0 or 2 cands, no bug1 exist!");
		return 0;
	}
	vector<int> cands=tc->getCands(ST_NORMAL);
	for(unsigned int ci=0;ci<cands.size();++ci)
	{
		int cand=cands[ci];
		int s=0;
		EnumSetType tps[]={ROW,COL,BOX};
		for(int tp=0;tp<=2;++tp) // tp is ROW,COL,BOX!
		{
			cells=tc->getConnectCellset(tps[tp])->getCells();
			s=0;
			for(unsigned int cj=0;cj<cells.size();++cj)
			{
				Cell *cell=cells[cj];
				if(cell->getCandStatus(cand)==ST_NORMAL)
					s++;
			}
			if(s!=3)
			{
				plog("Candidate %d is not target number!",cand);
				break;
			}
		}
		if(s==3)
		{
			plog("We found a cell is Bug1 cell:%s", tc->getName());
			//Found Bug1 Operation
			Operation op(ACT_SET);
			op.pts.push_back(tc->getPoint());
			op.nums.push_back(cand);
			SAction act(S_BUG1);
			act.ops.push_back(op);
			acts.push_back(act);
			return 1;
		}
	}
	return 0;
}

int Solver::tryHiddenNumberSet(Puzzle &pz,vector<SAction> &acts,int argDim)
{
	int ret=0;
	vector<CellSet *> css=pz.getCellSetList();
	for(unsigned int cssi=0;cssi<css.size();++cssi)
	{
		CellSet *cs=css[cssi];
		vector<Cell*> cells=cs->getCells();
		int sum[NUM][NUM];
		memset(sum,0,sizeof(sum));
		for(int cand=1;cand<=NUM;++cand)
		{
			for(unsigned int ci=0;ci<cells.size();++ci)
			{
				Cell *cell=cells[ci];
				if(cell->getCandStatus(cand)==ST_NORMAL)
				{
					sum[cand-1][ci]=1;
				}
			}
		}

		int dims[]={2,3,4};
		EnumSolver solvers[]={S_HiddenPair, S_HiddenTriplet, S_HiddenQuad};
		//for(int di=0;di<sizeof(dims)/sizeof(int);++di)
		{
			//int nDim=dims[di];
			int nDim=argDim;
			vector<vector<int> >& vv=vvv[nDim-2];
			for(unsigned int vvi=0;vvi<vv.size();++vvi)
			{
				vector<int>& v=vv[vvi];
				vector<int> cands=v;
				v_plus(cands,1);

				int n;
				//check sum
				int vs[NUM];
				memset(vs,0,sizeof(vs));
				int s=0;
				for(unsigned int vi=0;vi<v.size();++vi)
				{
					n=v[vi];
					s=0;
					for(int i=0;i<NUM;++i)
					{
						if(sum[n][i])
						{
							s++;
							vs[i]=1;
							if(s>nDim) break;
						}
					}
					if(s>nDim || s<2) break;
				}
				if(s!=nDim) continue;

				//check postion
				n=0;
				vector<Cell*> nscells;
				for(int i=0;i<NUM;++i)
				{
					int pi=vs[i];
					if(pi)
					{
						n+=pi;
						nscells.push_back(cells[i]);//TODO:BUG here
					}
				}
				if(n==nDim)
				{
					//plog(dumpstr(nscells).c_str());
					//plog("Found %d NumberSet(%s) @ %s!",nDim,dumpstr(cands).c_str(),cs->getName());
					SAction act(solvers[nDim-2]);
					act.cellsets.push_back(cs);
					act.nums=cands;
					act.pts=getPoints(nscells);
					vector<Cell *> ccells=nscells[0]->getConnectCells();
					ccells.push_back(nscells[0]);//do not lost itself
					for(unsigned int cci=0;cci<ccells.size();cci++)
					{
						int b=1;
						int f=0;
						Cell *cell=ccells[cci];
						if(cell->getValue()!=NUM_NONE) continue;
						//plog("Check cell:%s",cell->getName());
						for(unsigned int nsi=0;nsi<nscells.size();nsi++)
						{
							if( nscells[nsi] == cell)
							{
								f=1;
								break;
							}
							if( ! nscells[nsi]->isConnectTo(cell))
							{
								b=0;
								break;
							}
						}
						if(!b)
							continue;
						vector<int> tcands=cell->getCands(ST_NORMAL);
						if(!f)
						{
							//cell is connected to these nscells, and not one of them. Next, check its candidates, we should remove cands 'v'
							tcands=vv_intersection(tcands,cands);
							if(tcands.size())
							{
								Operation op(ACT_TAG);
								op.pts.push_back(cell->getPoint());
								op.nums=cands;
								act.ops.push_back(op);
								plog("Because:%dNumberSet@%s, we should remove %s from %s", nDim, cs->getName(), dumpstr(cands).c_str(), cell->getName());
							}
						}
						else
						{
							//cell is one of these nscells, we should remove cands not 'v'
							tcands=vv_difference(tcands,cands);
							if(tcands.size())
							{
								Operation op(ACT_TAG);
								op.pts.push_back(cell->getPoint());
								op.nums=tcands;
								act.ops.push_back(op);
								plog("Because:%dNumberSet@%s, we should remove %s from %s", nDim, cs->getName(), dumpstr(tcands).c_str(), cell->getName());
							}
						}
					}
					if(act.ops.size()>0)
					{
						acts.push_back(act);
						ret++;
						//if(nDim>2)
						return ret;
					}
				}
			}
		}
	}
	return ret;
}

int Solver::tryNakedNumberSet(Puzzle &pz,vector<SAction> &acts,int argDim)
{
	int ret=0;
	vector<CellSet*> css=pz.getCellSetList();
	int dims[]={2,3,4};
	EnumSolver solvers[]={S_NakedPair, S_NakedTriplet, S_NakedQuad};
	//for(int di=0;di<sizeof(dims)/sizeof(int);++di)
	{
		//int nDim=dims[di];
		int nDim=argDim;
		vector<vector<int> >& vv=vvv[nDim-2];
		for(unsigned int vvi=0;vvi<vv.size();++vvi)
		{
			vector<int>& v=vv[vvi];
			vector<int> cands=v;
			v_plus(cands,1);
			for(unsigned int cssi=0;cssi<css.size();++cssi)
			{
				CellSet *cs=css[cssi];
				vector<Cell*> cells=cs->getCells();
				vector<SPoint> pts;
				//plog("Scanning CellSet:%s @ %u", cs->getName(), GetTickCount());
				for(unsigned int ci=0;ci<cells.size();++ci)
				{
					Cell *cell=cells[ci];
					if(cell->getValue()!=NUM_NONE) continue;
					vector<int> ccands=cell->getCands(ST_NORMAL);
					if(ccands.size()>(unsigned int)nDim) continue;
					if(vv_contain(cands,ccands))
					{
						pts.push_back(cell->getPoint());
						if(pts.size()==nDim)
						{
							//plog("Found Naked %d NumberSet(%s) in %s @ %s", nDim, dumpstr(cands).c_str(), dumpstr(pts).c_str(), cs->getName());
							vector<Cell *> tcells=pz.getCell(pts[0])->getConnectCells();
							vector<Cell *> scells=getCells(pz,pts);
							vector<Operation> ops;
							for(unsigned int ti=0;ti<tcells.size();++ti)
							{
								Cell *tcell=tcells[ti];
								if(tcell->getValue()!=NUM_NONE) continue;
								int b=1;
								for(unsigned int si=0;si<scells.size();++si)
								{
									if( scells[si] == tcell ) //for Naked , we can't remove any candidates from these cells self
									{
										b=0;
										break;
									}
									if( ! scells[si]->isConnectTo(tcell) )
									{
										b=0;
										break;
									}
								}
								if(!b) continue;
								vector<int> tcands=tcell->getCands(ST_NORMAL);
								tcands=vv_intersection(tcands,cands);
								if(tcands.size())
								{
									Operation op(ACT_TAG);
									op.pts.push_back(tcell->getPoint());
									op.nums=tcands;
									ops.push_back(op);
									plog("Remove %s from %s, because Cells:%s @ %s", dumpstr(tcands).c_str(), tcell->getName(), dumpstr(pts).c_str(), cs->getName());
								}
							}
							if(ops.size())
							{
								SAction act(solvers[nDim-2]);
								act.cellsets.push_back(cs);
								act.nums=cands;
								act.pts=pts;
								act.ops=ops;
								acts.push_back(act);
								ret++;
								//we should return here, to avoid too much loops and try to find out simple SAction!
								//if(nDim>2)
								return ret;
							}
						}
					}
				}
			}
		}
	}
	return ret;
}


//X-Wing, Sword-Fish, Jelly-Fish
int Solver::tryFish(Puzzle &pz,vector<SAction> &acts,int argDim)
{
	int ret=0;
	int cstype[]={ROW,COL};
	int dims[]={2,3,4};
	EnumSolver solvers[]={S_XWing, S_SwordFish, S_JellyFish};


	int nCands[NUMROW][NUMCOL][NUM];
	memset(nCands,0,sizeof(nCands));
	vector<Cell *> allcells=pz.getCells();
	for(unsigned int ai=0;ai<allcells.size();++ai)
	{
		Cell* cell=allcells[ai];
		if(cell->getValue()!=NUM_NONE) continue;
		vector<int> cands=cell->getCands(ST_NORMAL);
		SPoint pt=cell->getPoint();
		int cx=pt.getx();
		int cy=pt.gety();
		for(unsigned int ci=0;ci<cands.size();++ci)
		{
			int cand=cands[ci];
			nCands[cx][cy][cand-1]=1;
		}
	}

	int tp=ROW;
	do
	{
		//for(int di=0;di<sizeof(dims)/sizeof(int);++di)
		{
			//int nDim=dims[di];
			int nDim=argDim;
			vector< vector<int> > vv=vvv[nDim-2];
			vector<CellSet *> cellsets;
			for(unsigned int vvi=0;vvi<vv.size();++vvi)
			{
				vector<int> v=vv[vvi];
				//if (nDim==2) 
				//	plog("Check cs:%s",dumpstr(v).c_str());
				for(int n=1;n<=NUM;++n)
				{
					int sum[NUM];
					memset(sum,0,sizeof(sum));
					vector<SPoint> pts;
					int s;
					for(unsigned int vi=0;vi<v.size();++vi)
					{
						int p1=v[vi];
						s=0;
						for(int p2=0;p2<NUM;p2++)
						{
							if(tp==ROW)
							{
								if(nCands[p1][p2][n-1])
								{
									s++;
									sum[p2]++;
									pts.push_back(SPoint(p1,p2));
									if(s>nDim)
										break;
								}
							}else if(tp==COL)
							{
								if(nCands[p2][p1][n-1])
								{
									s++;
									sum[p2]++;
									pts.push_back(SPoint(p2,p1));
									if(s>nDim)
										break;
								}
							}
							else plog("ERROR");
						}
						if(s>nDim || !s) 
							break; // try next vv
					}
					if(s>nDim || !s) 
						continue;  //try next cand
					int ss=0;
					for(int si=0;si<NUM;si++)
						if(sum[si])
							ss++;
					if(ss!=nDim) continue; //try next cand
					plog("Found Fish[%d] @ Cell:%s tp:%d", n, dumpstr(pts).c_str(), tp);
					vector<SPoint> tpts;
					for(int si=0;si<NUM;si++)
					{
						if(sum[si])
						{
							CellSet *cs=(tp==ROW)?pz.getCellSet(COL,si):pz.getCellSet(ROW,si);
							vector<Cell *> cells=cs->getCells();
							for(unsigned int ci=0;ci<cells.size();++ci)
							{
								Cell *cell=cells[ci];
								SPoint pt=cell->getPoint();
								if(vv_contain(pts,pt))
									continue;
								if(cell->getCandStatus(n)==ST_NORMAL){
									//plog("We can remove %d from %s, because %s", n, pt.getName(), dumpstr(pts).c_str() );
									tpts.push_back(pt);
								}
							}
						}
					}
					if(tpts.size()>0)
					{
						plog("Found %s[%d] @ Cell:%s tp:%d, remove %d from %s", getSolverName(solvers[nDim-2]), n, dumpstr(pts).c_str(), tp, n, dumpstr(tpts).c_str() );
						Operation op(ACT_TAG);
						op.pts=tpts;
						op.nums.push_back(n);
						SAction act(solvers[nDim-2]);
						act.pts=pts;
						set<CellSet*> css;
						for(unsigned int i=0;i<pts.size();++i)
						{
							if(tp==ROW)
								css.insert(_pz.getCellSet(ROW,pts[i].getx()));
							else
								css.insert(_pz.getCellSet(COL,pts[i].gety()));
						}
						act.cellsets=vector<CellSet *>(css.begin(),css.end());
						act.nums.push_back(n);
						act.ops.push_back(op);
						acts.push_back(act);
						ret++;
						return ret;
					}
				}
			}
		}
		if(tp==COL) break;
		tp=COL;// loop again
	}while(1);
	return ret;
}

int Solver::tryXYWing(Puzzle &pz,vector<SAction> &acts)
{
	int ret=0;
	vector<Cell *> tcells;
	vector< vector<int> > tcands;
	vector<Cell *> cells=pz.getCells();
	for(unsigned int ci=0;ci<cells.size();++ci)
	{
		Cell *cell=cells[ci];
		if(cell->getValue()!=NUM_NONE) continue;
		vector<int> cands=cell->getCands(ST_NORMAL);
		if(cands.size()!=2) continue;
		tcells.push_back(cell);
		tcands.push_back(cands);
	}
	if(!tcells.size())
		return 0; 
	for(unsigned int i=0;i<tcells.size()-1;++i)
	{
		Cell *cci=tcells[i];
		vector<int>& nci=tcands[i];
		for(unsigned int j=i+1;j<tcells.size();++j)
		{
			Cell *ccj=tcells[j];
			if(!ccj->isConnectTo(cci)) continue;
			vector<int>& ncj=tcands[j];
			if(nci[0] != ncj[0] && nci[0] !=ncj[1] && nci[1] !=ncj[0] && nci[1] != ncj[1])
			{
				continue;
			}
			if(nci[0] == ncj[0] && nci[1] == ncj[1])
			{
				plog("Naked Pair ?!?!?");
				continue;
			}
			int x,y,z;
			if(nci[0]==ncj[0])
			{
				z=nci[0];
				if(nci[1]<ncj[1]) {x=nci[1];y=ncj[1];} else {y=nci[1];x=ncj[1];}
			}
			else if(nci[0]==ncj[1])
			{
				z=nci[0];
				if(nci[1]<ncj[0]) {x=nci[1];y=ncj[0];} else {y=nci[1];x=ncj[0];}
			}
			else if(nci[1]==ncj[0])
			{
				z=nci[1];
				if(nci[0]<ncj[1]) {x=nci[0];y=ncj[1];} else {y=nci[0];x=ncj[1];}
			}
			else if(nci[1]==ncj[1])
			{
				z=nci[1];
				if(nci[0]<ncj[0]) {x=nci[0];y=ncj[0];} else {y=nci[0];x=ncj[0];}
			}
			for(unsigned int tci=0;tci<tcands.size();++tci)
			{
				if(tcands[tci][0]==x && tcands[tci][1]==y)
				{
					Cell *cck=tcells[tci];
					Cell *cct=NULL;
					Cell *ccc=NULL;
					int cand;
					if(cck->isConnectTo(cci))
					{
						cct=ccj;
						ccc=cci;
					}else if(cck->isConnectTo(ccj))
					{
						cct=cci;
						ccc=ccj;
					}
					else
						continue;
					if(cck->getCandStatus(x)==ST_NORMAL && cct->getCandStatus(x)==ST_NORMAL)
						cand=x;
					else if(cck->getCandStatus(y)==ST_NORMAL && cct->getCandStatus(y)==ST_NORMAL)
						cand=y;
					else
						continue;
					cells=cck->getConnectCells();
					vector<SPoint> pts;
					for(unsigned int i=0;i<cells.size();++i)
					{
						Cell *cell=cells[i];
						if(cell->isConnectTo(cct))
						{
							vector<int> cands=cell->getCands(ST_NORMAL);
							if(cands.size()==2 && cands[0]== x && cands[1] == y)
							{
								pts.push_back(cell->getPoint());
							}
						}
					}
					if(pts.size())
					{
						Operation op(ACT_TAG);
						op.pts=pts;
						op.nums.push_back(cand);
						SAction act(S_XYWing);
						act.ops.push_back(op);
						act.pts.push_back(ccc->getPoint());
						act.pts.push_back(cck->getPoint());
						act.pts.push_back(cct->getPoint());
						act.nums.push_back(z);
						act.nums.push_back(x);
						act.nums.push_back(y);
						acts.push_back(act);
						plog("We have found a xy-wing: %d,%d,%d @ %s,%s,%s ,Operation: remove %d @ %s", x, y, z, cck->getName(), cci->getName(), ccj->getName(), cand, dumpstr(op.pts).c_str() );
						ret++;
						return ret;
					}
				}
			}
		}
	}
	return ret;
}

int Solver::tryXYZWing(Puzzle &pz,vector<SAction> &acts)
{
	int ret=0;
	vector<Cell *> cells=pz.getCells();
	vector<Cell *> cells2;
	vector<Cell *> cells3;
	for(unsigned int ci=0;ci<cells.size();++ci)
	{
		Cell *cell=cells[ci];
		if(cell->getValue()!=NUM_NONE) continue;
		vector<int> cands=cell->getCands(ST_NORMAL);
		switch(cands.size())
		{
		case 2:
			cells2.push_back(cell);
			break;
		case 3:
			cells3.push_back(cell);
			break;
		default:
			continue;
		}
	}
	for(unsigned int i2=0;i2<cells2.size();++i2)
	{
		Cell *cell2=cells2[i2];
		vector<int> cands2=cell2->getCands(ST_NORMAL);
		for(unsigned int i3=0;i3<cells3.size();++i3)
		{
			Cell *cell3=cells3[i3];
			if(!cell3->isConnectTo(cell2))
				continue;
			if(cell2->getConnectCellset(BOX)==cell3->getConnectCellset(BOX))
				continue; //can not in same BOX
			vector<int> cands3=cell3->getCands(ST_NORMAL);
			if(vv_contain(cands3,cands2))
			{
				vector<int> candsz=vv_difference(cands3,cands2);
				if(candsz.size()==1)
				{
					int z=candsz[0];
					int x,y;
					vector<Cell *> bcells=cell3->getConnectCellset(BOX)->getCells();
					for(unsigned int ib=0;ib<bcells.size();++ib)
					{
						Cell *bcell=bcells[ib];
						if(bcell==cell3 || bcell->isConnectTo(cell2))
							continue;
						vector<int> bcands=bcell->getCands(ST_NORMAL);
						if(bcands.size()!=2)
							continue;
						if(!vv_contain(bcands,z))
							continue;
						if(vv_contain(bcands,cands2[0]))
						{
							x=cands2[1];
							y=cands2[0];
						}else if(vv_contain(bcands,cands2[1]))
						{
							x=cands2[0];
							y=cands2[1];
						}
						else continue;
						//bcands is y,z,  cands2 is x,y, cands is x,y,z
						vector<SPoint> pts;
						for(unsigned int i=0;i<bcells.size();++i)
						{
							Cell *bc=bcells[i];
							if(bc->isConnectTo(cell2) && bc!=cell3 && bc->getCandStatus(y)==ST_NORMAL)
							{
								//Found xyz-wing removes
								pts.push_back(bc->getPoint());
							}
						}
						if(pts.size()>0)
						{
							Operation op(ACT_TAG);
							op.pts=pts;
							op.nums.push_back(y);
							SAction act(S_XYZWing);
							act.ops.push_back(op);
							act.nums.push_back(x);
							act.nums.push_back(y);
							act.nums.push_back(z);
							act.pts.push_back(cell2->getPoint());
							act.pts.push_back(cell3->getPoint());
							act.pts.push_back(bcell->getPoint());
							//act.cellsets.push_back(cell3->getConnectCellset(BOX));
							acts.push_back(act);
							ret++;
							return ret;
						}
					}
				}
			}
		}
	}
	return ret;
}

int Solver::tryUniqueRectangle(Puzzle &pz,vector<SAction> &acts)
{
	int ret=0;
	//find rectangle first, then count cells which cands.size() >0 , calc which cands are extra, find intersection of connected cells from these cells (cands.size>0)
	vector< vector<int> >& vv=vvv[0];
	for(unsigned int ix=0;ix<vv.size();++ix)
	{
		vector<int>& vx=vv[ix];
		for(unsigned int iy=0;iy<vv.size();++iy)
		{
			vector<int>& vy=vv[iy];
			int lt=cord2boxid(vx[0],vy[0]);
			int rb=cord2boxid(vx[1],vy[1]);
			if(lt==rb) continue;
			int rt=cord2boxid(vx[1],vy[0]);
			int lb=cord2boxid(vx[0],vy[1]);
			if(lt!=rt && lt !=lb) continue;
			vector<Cell *> rcells;
			rcells.push_back(pz.getCell(vx[0],vy[0]));
			rcells.push_back(pz.getCell(vx[0],vy[1]));
			rcells.push_back(pz.getCell(vx[1],vy[0]));
			rcells.push_back(pz.getCell(vx[1],vy[1]));
			vector<int> xy;
			vector< vector<int> > rcands;
			int bchk=1;
			vector< Cell *> fcells;
			for(unsigned int ir=0;ir<rcells.size();++ir)
			{
				vector<int> cands=rcells[ir]->getCands(ST_NORMAL);
				if(cands.size()==2)
				{
					xy=cands;
				}
				else
				{
					fcells.push_back(rcells[ir]);
				}
				rcands.push_back(cands);
			}
			for(unsigned int ir=0;ir<rcells.size();++ir)
			{
				vector<int> cands=rcells[ir]->getCands(ST_NORMAL);
				if(!vv_contain(cands,xy))
				{
					bchk=0;
					break;
				}
			}
			if(!bchk)
				continue;
			if(fcells.size()==1)
			{
				plog("Found UniqueRectangle1");
				Cell *cell=fcells[0];
				vector<int> cands=cell->getCands(ST_NORMAL);
				cands=vv_difference(cands,xy);
				SAction act(S_UniqueRectangle1);
				act.pts=getPoints(rcells);
				act.nums=xy;
				if(cands.size()==1)
				{
					Operation op(ACT_SET);
					op.nums.push_back(cands[0]);
					op.pts.push_back(cell->getPoint());
					act.ops.push_back(op);
					acts.push_back(act);
				}
				else
				{
					Operation op(ACT_TAG);
					op.nums=xy;
					op.pts.push_back(cell->getPoint());
					act.ops.push_back(op);
					acts.push_back(act);
				}
				ret++;
				return ret;
			}else if(fcells.size()==3)
			{
				Cell *c1=fcells[0];
				Cell *c2=fcells[1];
				Cell *c3=fcells[2];
				Cell *cc;
				if(!c1->isConnectTo(c2))
				{
					cc=c3;
				}
				else
				{
					if(!c1->isConnectTo(c3))
						cc=c2;
					else
						cc=c1;
				}
				vector<int> scr=v_countcands(cc->getConnectCellset(ROW));
				vector<int> scc=v_countcands(cc->getConnectCellset(COL));
				do 
				{
					int cand;
					if(scr[xy[0]-1]==2 && scc[xy[0]-1]==2)
						cand=xy[1];
					else if(scr[xy[1]-1]==2 && scc[xy[1]-1]==2)
						cand=xy[0];
					else
						break;
					Operation op(ACT_TAG);
					op.pts.push_back(cc->getPoint());
					op.nums.push_back(cand);
					SAction act(S_UniqueRectangle7);
					act.pts=getPoints(rcells);
					act.ops.push_back(op);
					act.nums=xy;
					act.cellsets.push_back(cc->getConnectCellset(ROW));
					act.cellsets.push_back(cc->getConnectCellset(COL));
					acts.push_back(act);
					ret++;
					return ret;
				} while (false);
				//go on to check UR5
			}
			else if(fcells.size()==2)
			{
				SPoint pt1=fcells[0]->getPoint();
				SPoint pt2=fcells[1]->getPoint();
				if(pt1.getx() != pt2.getx() && pt1.gety() !=pt2.gety())
				{
					Cell *c1=fcells[0];
					Cell *c2=fcells[1];
					EnumSetType tp=ROW;
					CellSet *cs1=c1->getConnectCellset(tp);
					CellSet *cs2=c2->getConnectCellset(tp);
					vector<int> sc1=v_countcands(cs1);
					vector<int> sc2=v_countcands(cs2);
					if( (sc1[xy[0]-1]!=2 || sc2[xy[0]-1]!=2)  && (sc1[xy[1]-1]!=2 || sc2[xy[1]-1]!=2) )
					{
						tp=COL;
						cs1=c1->getConnectCellset(tp);
						cs2=c2->getConnectCellset(tp);
						sc1=v_countcands(cs1);
						sc2=v_countcands(cs2);
					}
					do 
					{
						int cand;
						if( sc1[xy[0]-1]==2 && sc2[xy[0]-1]==2  )
						{
							cand=xy[0];
						}
						else if (sc1[xy[1]-1]==2 && sc2[xy[1]-1]==2)
						{
							cand=xy[1];
						}
						else
							break;
						plog("Found UR6");
						Operation op(ACT_TAG);
						op.nums.push_back(cand);
						op.pts.push_back(c1->getPoint());
						op.pts.push_back(c2->getPoint());
						SAction act(S_UniqueRectangle6);
						act.ops.push_back(op);
						act.pts=getPoints(rcells);
						act.nums=xy;
						acts.push_back(act);
						ret++;
						return ret;
					} while (false);
					
					continue; // fcells are not in same ROW and COL, skip
				}


				//Try UR4
				//if(fcells.size()==2)
				{
					CellSet *cs;
					Cell *fc1=fcells[0];
					Cell *fc2=fcells[1];
					if(fc1->getConnectCellset(ROW)==fc2->getConnectCellset(ROW))
						cs=fc1->getConnectCellset(ROW);
					else
						cs=fc1->getConnectCellset(COL);
					//int sum[NUM];
					//memset(sum,0,sizeof(sum));
					//vector<Cell *> ccells=cs->getCells();
					//for(unsigned int i=0;i<ccells.size();++i)
					//{
					//	vector<int> ccands=ccells[i]->getCands(ST_NORMAL);
					//	for(unsigned int j=0;j<ccands.size();++j)
					//	{
					//		sum[ccands[j]-1]++;
					//	}
					//}
					vector<int> sum=v_countcands(cs);
					do 
					{
						int cand;
						if(sum[xy[0]-1]==2)
						{
							cand=xy[1];
						}
						else if(sum[xy[1]-1]==2)
						{
							cand=xy[0];
						}
						else 
							break;
						Operation op(ACT_TAG);
						op.pts.push_back(fc1->getPoint());
						op.pts.push_back(fc2->getPoint());
						op.nums.push_back(cand);
						SAction act(S_UniqueRectangle4);
						act.pts=getPoints(rcells);
						act.ops.push_back(op);
						act.nums=xy;
						act.cellsets.push_back(cs);
						acts.push_back(act);
						ret++;
						return ret;
					} while (false);

				}
			}
			//after works above, fcells.size() >=2 ! and at least two cells are in same ROW/COL
			set<int> fs;
			for(unsigned int ir=0;ir<fcells.size();++ir)
			{
				vector<int> cands=fcells[ir]->getCands(ST_NORMAL);
				cands=vv_difference(cands,xy);
				fs.insert(cands.begin(),cands.end());
			}
			vector<int> fcands(fs.begin(),fs.end()); //fcands has removed xy!
			vector<Cell *> fcs=fcells[0]->getConnectCells();
			vector<Cell*> tcells; // cells which connected to fcells!
			for(unsigned int ir=0;ir<fcs.size();++ir)
			{
				Cell *cell=fcs[ir];
				if(cell->isConnectTo(fcells) && !vv_contain(fcells,cell))
				{
					tcells.push_back(cell);
				}
			}
			//try to find singlenumber, numberset 
			if(fcands.size()==1) 
			{
				int cand=fcands[0];
				vector<SPoint> pts;
				for(unsigned int i=0;i<tcells.size();++i)
				{
					Cell *cell=tcells[i];
					if(cell->getCandStatus(cand)==ST_NORMAL)
					{
						pts.push_back(cell->getPoint());
					}
				}
				if(pts.size()>0)
				{
					plog("Found UniqueRectange2/5!");
					Operation op(ACT_TAG);
					op.pts=pts;
					op.nums.push_back(cand);
					SAction act(S_UniqueRectangle2);
					act.pts=getPoints(rcells);
					act.nums=xy;
					act.ops.push_back(op);
					acts.push_back(act);
					ret++;
					return ret;
				}
			}
			else if(fcands.size() ==2)
			{
				//only try to find naked 2,3 numberset
				//The first cell is virtual cell, it comes from fcells
				//tcells are cells which connected to fcells
				for(unsigned int i=0;i<tcells.size();++i)
				{
					Cell *cell2=tcells[i];
					if(cell2->getValue()!=NUM_NONE) continue;
					vector<int> cands2=cell2->getCands(ST_NORMAL);
					vector<int> cands=vv_union(cands2,fcands);
					if(cands.size()==2) //Found NakedPair
					{
						vector<SPoint> pts;
						for(unsigned int j=0;j<tcells.size();++j)
						{
							if(i==j) continue;
							Cell *tc=tcells[j];
							if(tc->isConnectTo(cell2) )
							{
								if(tc->getCandStatus(fcands[0])==ST_NORMAL||
									tc->getCandStatus(fcands[1])==ST_NORMAL)
									pts.push_back(tc->getPoint());
							}
						}
						if(pts.size())
						{
							Operation op(ACT_TAG);
							op.nums=cands;
							op.pts=pts;
							SAction act(S_UniqueRectangle3);
							act.pts=getPoints(rcells);
							act.pts.push_back(cell2->getPoint());
							act.nums=xy;
							act.ops.push_back(op);
							acts.push_back(act);
							ret++;
							return ret;
						}
					}
					if(cands.size()>3) continue;
					for(unsigned int j=i+1;j<tcells.size();++j)
					{
						Cell *cell3=tcells[j];
						if(cell3->getValue()!=NUM_NONE) continue;
						if(!cell3->isConnectTo(cell2)) continue;
						vector<int> cands3=cell3->getCands(ST_NORMAL);
						vector<int> cands3b=vv_union(cands,cands3);
						if(cands3b.size()==3)
						{
							vector<SPoint> pts;
							for(unsigned int k=0;k<tcells.size();++k)
							{
								if(k==i || k==j) continue;
								Cell *tc=tcells[k];
								if(tc->isConnectTo(cell2) && tc->isConnectTo(cell3))
								{
									if(tc->getCandStatus(fcands[0])==ST_NORMAL||
										tc->getCandStatus(fcands[1])==ST_NORMAL)
										pts.push_back(tc->getPoint());
								}
							}
							if(pts.size())
							{
								Operation op(ACT_TAG);
								op.nums=cands3b;
								op.pts=pts;
								SAction act(S_UniqueRectangle3);
								act.pts=getPoints(rcells);
								act.pts.push_back(cell2->getPoint());
								act.pts.push_back(cell3->getPoint());
								act.nums=xy;
								act.ops.push_back(op);
								acts.push_back(act);
								ret++;
								return ret;
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

void Solver::resetNodes()
{
	SNode nn;
	for(int x=0;x<NUM;++x)
		for(int y=0;y<NUM;++y)
			for(int n=0;n<NUM;++n)
				_nodes[x][y][n]=nn;//SNode();
}

void Solver::updateNodes()
{
	for(int x=0;x<NUM;++x)
	{
		for(int y=0;y<NUM;++y)
		{
			updateNodes(x,y);
		}
	}
}

void Solver::updateNodes(int x,int y)
{

	//PRPR("updateNodes");

	if(_nodes[x][y][0].isValid())
		return;
	for(int n=0;n<NUM;++n)
		_nodes[x][y][n]=SNode(x,y,n);

	Cell *cell=_pz.getCell(x,y);
	if(cell->getValue()!=NUM_NONE)
	{
		//continue;
		return;
	}
	vector<int>cands=cell->getCands(ST_NORMAL);
	if(cands.size()==2)
	{
		//strong link
		_nodes[x][y][cands[0]-1].link(x,y,cands[1],true);
		_nodes[x][y][cands[1]-1].link(x,y,cands[0],true);
	}
	for(unsigned int i=0;i<cands.size();++i)
	{
		for(unsigned int j=0;j<cands.size();++j)
		{
			if(i==j) continue;
			//weak link
			_nodes[x][y][cands[i]-1].link(x,y,cands[j],false);
		}
	}
	EnumSetType tps[]={ROW,COL,BOX};
	for(int ti=0;ti<sizeof(tps)/sizeof(tps[0]);++ti)
	{
		EnumSetType tp=tps[ti];
		vector<Cell *> cells=cell->getConnectCellset(tp)->getCells();
		vector<int> sumcands=v_countcands(cells);
		for(unsigned int i=0;i<cands.size();++i)
		{
			int cand=cands[i];
			//if(sumcands[cand-1]==0) continue;//This won't happen!
			bool bStrongLink=false;
			if(sumcands[cand-1]==2) bStrongLink=true;
			for(unsigned int j=0;j<cells.size();++j)
			{
				Cell *tc = cells[j];
				SPoint pt=tc->getPoint();
				if(pt.equal(x,y)) continue;
				//PRST("AAA");
				if(tc->getCandStatus(cand)==ST_NORMAL)
				{
					if(bStrongLink)
						_nodes[x][y][cand-1].link(pt.getx(),pt.gety(),cand,true);
					_nodes[x][y][cand - 1].link(pt.getx(), pt.gety(), cand, false);
					//PRST("BBB");
				}
				//PRST("CCC");
			}
		}
	}
}



int Solver::shortChain( vector< vector<int> > &chain )
{
	bool bErased=false;
	do
	{
		bErased=false;
		unsigned int nLen=chain.size();
		for(unsigned int i=0;i<nLen-1 && !bErased;++i)
		{
			vector<int> cn=chain[i];
			SNode &node =_nodes[cn[0]][cn[1]][cn[2]-1];
			vector< vector<int> > links[]={node._weaklink, node._stronglink};
			vector< vector<int> > &lnk=links[cn[3]];
			for(unsigned int k=0;k<lnk.size() && !bErased;++k)
			{
				for(unsigned int j=i+2;j<chain.size();++j)
				{
					vector<int> nn=chain[j];
					if(nn[3]!=cn[3] && vv_equal(lnk[k],nn,3))
					{
						plog("Found shortable chain! ");
						plog("Before:%s",dumpChain(chain,chain[0]).c_str());
						chain.erase(chain.begin() + i+1,chain.begin() + j);
						plog(" After:%s",dumpChain(chain,chain[0]).c_str());
						bErased=true;
						break;
					}
				}
			}
		}
	}while(bErased);
	return !bErased;
}

int findNode(vector< vector<int> > &chain, int x, int y, int cand,int &linktype)
{
	for(unsigned int i=0;i<chain.size();++i)
	{
		vector<int> &n=chain[i];
		if(n[0]==x && n[1]==y && n[2]==cand)
		{
			linktype=n[3];
			return i;
		}
	}
	return -1;
}

int dumpflag=0;
int Solver::findChain(EnumSolver solver, vector<SAction> &acts, vector< vector<int> > &chain,int r,int x, int y, int cand, bool bStrongLink)
{
	if (r > 10) return false;
	if (solver == S_XYChain && chain.size() > 0 &&  _pz.getCell(x,y)->getCands(ST_NORMAL).size()!=2)
		return false;
	vector<int> cn;
	cn.push_back(x);cn.push_back(y);cn.push_back(cand);cn.push_back(bStrongLink);
	chain.push_back(cn);
	if(dumpflag )//|| solver==S_XYChain ) 
		plog(dumpChain(chain, chain[0]).c_str());
	vector<int> pn;
	if(chain.size()>1)
	{
		pn=chain[chain.size()-2];
	}
	updateNodes(x,y);
	SNode node=_nodes[x][y][cand-1];

	if(bStrongLink)
	{
		if(node._stronglink.size())
		{
			for(unsigned int i=0;i<node._stronglink.size();++i)
			{
				vector<int> & nn = node._stronglink[i];
				if(solver==S_XChain && nn[2] != cand) continue;
				if(pn.size()==4 && vv_equal(pn,nn,3) && pn[3]==0)
					continue; //skipped: next node is prev node
				int linktype;
				int nExist=findNode(chain,nn[0],nn[1],nn[2],linktype);
				if(nExist==-1)
				{
					if(findChain(solver,acts,chain,r+1,nn[0],nn[1],nn[2], !bStrongLink))
						return true;
				}
				else
				{
					if(linktype)
					{
						plog("Found CHAIN:%s",dumpChain(chain, nn).c_str());
						vector< vector<int> > tchain=chain;
						while(1)
						{
							if(vv_equal(tchain[0],nn,3))
								break;
							tchain.erase(tchain.begin());
						}
						size_t nsz=tchain.size();
						if(nsz<3)
						{
							plog("ERROR: break");
							plog("ERROR: break");
						}
						SPoint pt1(tchain[1][0],tchain[1][1]);
						SPoint pt2(tchain[nsz-1][0],tchain[nsz-1][1]);
						Operation op(ACT_SET);
						op.nums.push_back(nn[2]);
						op.pts.push_back(SPoint(nn[0],nn[1]));
						SAction act(solver);
						act.ops.push_back(op);
						act.chain=tchain;
						acts.push_back(act);
						return true;
					}
				}
			}
		}
	}
	else
	{
		if(node._weaklink.size())
		{
			for(unsigned int i=0;i<node._weaklink.size();++i)
			{
				vector<int> & nn = node._weaklink[i];
				if(solver==S_XChain && nn[2] != cand) continue;
				if(pn.size()==4 && vv_equal(pn,nn,3) && pn[3])
					continue; //skipped: next node is prev node
				int linktype;
				int nExist=findNode(chain,nn[0],nn[1],nn[2],linktype);
				if(nExist==-1)
				{
					if(findChain(solver,acts,chain,r+1,nn[0],nn[1],nn[2], !bStrongLink))
						return true;
				}
				else
				{
					if(!linktype)
					{
						plog("Found CHAIN:%s",dumpChain(chain, nn).c_str());
						vector< vector<int> > tchain=chain;
						while(1)
						{
							if(vv_equal(tchain[0],nn,3))
								break;
							tchain.erase(tchain.begin());
						}
						size_t nsz=tchain.size();
						if(nsz<3)
						{
							plog("ERROR: break");
							plog("ERROR: break");
						}
						vector<int> &nhead=tchain[1];
						vector<int> &ntail=tchain[nsz-1];
						if(nhead[2]==ntail[2])
						{
							vector<SPoint> pts;
							SPoint pt1(tchain[1][0],tchain[1][1]);
							SPoint pt2(tchain[nsz-1][0],tchain[nsz-1][1]);
							Cell *c1=_pz.getCell(pt1);
							Cell *c2=_pz.getCell(pt2);
							vector<Cell *> tcells=c1->getConnectCells();

							for(unsigned int i1=0;i1<tcells.size();++i1)
							{
								Cell *tc=tcells[i1];
								if( !tc->isConnectTo(c2)) continue;
								if( tc->getCandStatus(cand)!=ST_NORMAL) continue;
								pts.push_back(tc->getPoint());
							}
							Operation op(ACT_TAG);
							op.nums.push_back(cand);
							op.pts=pts;
							SAction act(solver);
							act.chain=tchain;
							act.chain.erase(act.chain.begin());
							act.ops.push_back(op);
							acts.push_back(act);
							return true;
						}
						else 
						{
							Operation op(ACT_TAG);
							op.nums.push_back(nn[2]);
							op.pts.push_back(SPoint(nn[0],nn[1]));
							SAction act(solver);
							act.chain=tchain;
							act.chain.erase(act.chain.begin());
							act.ops.push_back(op);
							acts.push_back(act);
							return true;
						}
					}
				}
			}
		}
	}
	if(chain.size())
		chain.erase(--chain.end());
	return false;
}

int Solver::tryXChain(Puzzle &pz,vector<SAction> &acts)
{
	int ret=0;
	vector<Cell *> cells=pz.getCells();
	for(unsigned int i=0;i<cells.size();++i)
	{
		vector<int> cands=cells[i]->getCands(ST_NORMAL);
		SPoint pt=cells[i]->getPoint();
		for(unsigned int j=0;j<cands.size();++j)
		{
			int cand=cands[j];
			vector< vector<int> > chain;
			if(findChain(S_XChain,acts,chain,0,pt.getx(),pt.gety(),cand,true))
				return 1;
			chain.clear();
			if(findChain(S_XChain,acts,chain,0,pt.getx(),pt.gety(),cand,false))
				return 1;
		}
	}
	return ret;
}


int Solver::tryForcingChain(Puzzle &pz,vector<SAction> &acts)
{
	int ret=0;
	vector<Cell *> cells=pz.getCells();
	for(unsigned int i=0;i<cells.size();++i)
	{
		vector<int> cands=cells[i]->getCands(ST_NORMAL);
		SPoint pt=cells[i]->getPoint();
		for(unsigned int j=0;j<cands.size();++j)
		{
			int cand=cands[j];
			vector< vector<int> > chain;
			if(findChain(S_ForcingChain,acts,chain,0,pt.getx(),pt.gety(),cand,true))
				return 1;
			chain.clear();
			if(findChain(S_ForcingChain,acts,chain,0,pt.getx(),pt.gety(),cand,false))
				return 1;
		}
	}
	return ret;
}


int Solver::tryXYChain(Puzzle &pz,vector<SAction> &acts)
{	
	int ret=0;
	vector<Cell *> cells=pz.getCells();
	for(unsigned int i=0;i<cells.size();++i)
	{
		vector<int> cands=cells[i]->getCands(ST_NORMAL);
		//if(cands.size()!=2) continue;
		SPoint pt=cells[i]->getPoint();
		for(unsigned int j=0;j<cands.size();++j)
		{
			int cand=cands[j];
			vector< vector<int> > chain;
			if(findChain(S_XYChain,acts,chain,0,pt.getx(),pt.gety(),cand,true))
				return 1;
			chain.clear();
			if(findChain(S_XYChain,acts,chain,0,pt.getx(),pt.gety(),cand,false))
				return 1;
		}
	}
	return ret;
}