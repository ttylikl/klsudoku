#ifndef __FUNCS_TEMP_H__
#define __FUNCS_TEMP_H__

// VS2010 Migration
// error C3861: 'back_inserter': identifier not found
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#include <iterator>
#endif

inline vector<int> v_countcands(vector<Cell *> cells)
{
	int sum[NUM];
	memset(sum,0,sizeof(sum));
	for(unsigned int i=0;i<cells.size();++i)
	{
		vector<int>& cands=cells[i]->getCands(ST_NORMAL);
		for(unsigned int j=0;j<cands.size();++j)
		{
			sum[cands[j]-1]++;
		}
	}
	return vector<int>(sum,sum+NUM);
}

inline vector<int> v_countcands(CellSet *cs)
{
	return v_countcands(cs->getCells());
}

inline vector<SPoint> getPoints(vector<Cell*> &cells)
{
	vector<SPoint> ret;
	for(vector<Cell*>::iterator it=cells.begin();it!=cells.end();++it)
	{
		Cell *cell= *it;
		ret.push_back(cell->getPoint());
	}
	return ret;
}

inline vector<Cell *> getCells(Puzzle &pz, vector<SPoint> &pts)
{
	vector<Cell *> ret;
	for(vector<SPoint>::iterator it=pts.begin();it!=pts.end();++it)
	{
		ret.push_back(pz.getCell(*it));
	}
	return ret;
}

inline void v_plus(vector<int>&v,int n)
{
	for(unsigned int i=0;i<v.size();i++)
	{
		v[i]+=n;
	}
}


inline string dumpstr(vector<SPoint> &vpt)
{
	char tmp[32];
	if(!vpt.size()) return "";
	char *buf=(char*)malloc(vpt.size()*20);
	*buf=0;
	vector<SPoint>::iterator it=vpt.begin();
	while(it!=vpt.end())
	{
		//sprintf(tmp,"<%s>,", it++->getName());
		SPoint rc =xy2rc( *it++);
		sprintf(tmp, "R%dC%d", rc.getx(),rc.gety() );
		strcat(buf,tmp);
        if(it!=vpt.end()){
            strcat(buf, ",");
        }
	}
	string ret(buf);
	free(buf);
	return ret;
}

inline string dumpstr(vector<CellSet *> &vpt)
{
	char tmp[32];
	if(!vpt.size()) return "";
	char *buf=(char*)malloc(vpt.size()*20);
	*buf=0;
	vector<CellSet *>::iterator it=vpt.begin();
	while(it!=vpt.end())
	{
		sprintf(tmp,"<%s>", (*it++)->getName());
		strcat(buf,tmp);
        if(it!=vpt.end()){
            strcat(buf, ",");
        }
	}
	string ret(buf);
	free(buf);
	return ret;
}

inline string dumpstr(vector<int> &vi)
{
	char tmp[32]; 
	if(!vi.size()) return "";
	char *buf=(char*)malloc(vi.size()*20);
	*buf=0;
	vector<int>::iterator it=vi.begin();
	while(it!=vi.end())
	{
		sprintf(tmp,"%d",*it++);
		strcat(buf,tmp);
        if(it!=vi.end()){
            strcat(buf, ",");
        }
	}
	string ret(buf);
	free(buf);
	return ret;
}


template <typename T>
vector<T> vv_union(vector<T> &v1, vector<T> &v2)
{
	set<T> s1;
	set<T> s2;
	s1.insert(v1.begin(),v1.end());
	s2.insert(v2.begin(),v2.end());
	set<T> s;
	//set_union(v1.begin(),v1.end(),v2.begin(),v2.end(),s.begin());
	set_union(s1.begin(),s1.end(),s2.begin(),s2.end(),inserter(s,s.begin()));
	return vector<T> (s.begin(),s.end()) ;
}

template <class T>
vector<T> vv_difference(vector<T> &v1, vector<T> &v2)
{
	set<T> s1;
	set<T> s2;
	s1.insert(v1.begin(),v1.end());
	s2.insert(v2.begin(),v2.end());
	set<T> s;
	set_difference(s1.begin(),s1.end(),s2.begin(),s2.end(),inserter(s,s.begin()));
	return vector<T> (s.begin(),s.end()) ;
}

template <class T>
vector<T> vv_intersection(vector<T> &v1, vector<T> &v2)
{
	set<T> s1;
	set<T> s2;
	s1.insert(v1.begin(),v1.end());
	s2.insert(v2.begin(),v2.end());
	set<T> s;
	set_intersection(s1.begin(),s1.end(),s2.begin(),s2.end(),inserter(s,s.begin()));
	return vector<T> (s.begin(),s.end()) ;
}

template <class T>
int vv_equal(vector<T> &v1, vector<T> &v2)
{
	//return vv_difference(v1,v2).size()==0;
	set<T> s1;
	set<T> s2;
	s1.insert(v1.begin(),v1.end());
	s2.insert(v2.begin(),v2.end());
	if(s1.size()!=s2.size()) return 0;
	set<T> s;
	set_difference(s1.begin(),s1.end(),s2.begin(),s2.end(),inserter(s,s.begin()));
	return s.size()==0;
}

template <class T>
int vv_equal(vector<T> &v1, vector<T> &v2, unsigned int len)
{
	if(v1.size()<len  || v2.size()<len) return false;
	for(unsigned int i=0;i<v1.size()&& i<len;++i)
	{
		if(v1[i]!=v2[i]) return false;
	}
	return true;
}

template <class T>
int vv_contain(vector<T> &v, T &t)
{
	for(unsigned int i=0;i<v.size();++i)
	{
		if(v[i] == t)
			return 1;
	}
	return 0;
}

template <class T>
inline int vv_contain(vector<T> &v1, vector<T> &v2)
{
	//return vv_equal(vv_intersection(v1,v2),v2);
	for(unsigned int i2=0;i2<v2.size();++i2)
	{
		if(!vv_contain(v1,v2[i2]))
			return 0;
	}
	return 1;
}




inline string dumpChain( vector< vector<int> > &chain, vector<int> &head)
{
	int hi;
	bool bDump=false;
	string str;
	for(unsigned int i=0;i<chain.size();++i)
	{
		if(vv_equal(chain[i],head,3))
		{
			hi=i;
			bDump=true;
		}
		if(!bDump) continue;
		vector<int> &node=chain[i];
		SPoint pt(node[0],node[1]);
		int cand=node[2];
		int bStrongLink=node[3];
		char buf[1024];
		const char *szlink="";
		if(i<chain.size()-1)
			szlink=node[3]?"=>":"->";
		SPoint rc = xy2rc(pt);
		sprintf(buf,"R%dC%d[%d]%s%s",rc.getx(),rc.gety(),cand,szlink,((i+1)%6)?"":"\n");
		str+=buf;
	}
	return str;
}


inline string dumpChain(vector< vector<int> > &chain)
{
	if(chain.size()==0)
		return "";
	return dumpChain(chain,chain[0]);
}


#endif
