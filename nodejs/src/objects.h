#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include "common.h"


//Notice: we use name to make resource filename!
//enum EnumColor { CBLUE = 0, CBLACK = 1, CTAG, CGREEN, CRED };
extern const char* g_stname[];// = { "blue", "black", "tag", "green", "red" };

class SPoint
{
private:
	char _name[10];
    int _x;
    int _y;
public:
	SPoint(const char *xystr){
		*_name = 0;
		setxy(xystr);
	}
    SPoint(int x,int y)
    {
		*_name = 0;
        setxy(x,y);
    }

	string dumps() {
		char buf[100];
		sprintf(buf,"{\"x\":%d,\"y\":%d}", _x+1, _y+1);
		return buf;
	}

	bool operator ==(const SPoint &pt) const
	{
		return equal(pt)!=0;
	}

    inline int equal(const SPoint &p) const
    {
        return equal(p.getx(),p.gety());
    }
	inline int equal(int cx, int cy) const
    {
        return _x==cx && _y==cy;
    }
    inline int getx() const
    {
        return _x;
    }
	inline int gety() const
    {
        return _y;
    }

	const char * getName()
	{
		if (*_name == 0)
		{
			sprintf(_name, "%d,%d", _y + 1, _x + 1); 
		}
		return _name;
	}

	int setxy(const char *xystr){
		char buf[100];
		strcpy(buf, xystr);
		char *p = strchr(buf, ',');
		*p = 0;
		p++;
		return setxy(atoi(p) - 1, atoi(buf) - 1); //according to getName() !:)
	}

	int setxy(int x, int y)
	{
		_x = x;
		_y = y;
		*_name = 0;

		return 0;
	}
};

class CellSet;
class Cell
{
public:
	Cell(int x,int y);

private:
	char _name[16];
    EnumStatus _cands[NUM]; //the status of candidates: ST_NONE, ST_TAG, ST_NORMAL(default)
    int _num; // store the Number of this cell. If there wasn't set, it should be NUM_NONE(0).
    SPoint _xy;
	vector<int> _vcands;
    vector<Cell *> _vcells; //store cells which connect with this.
    int _mark;
	CellSet * _cellset[3];
	int _bDirty;
public:
    int copyFrom(Cell *sc);
    EnumStatus getCandStatus(int cand);
    EnumStatus setCandStatus(int cand,EnumStatus st);
    vector<int>& getCands(EnumStatus st);
    vector<Cell*>& getConnectCells();
	CellSet *getConnectCellset(EnumSetType tp);
    int connect(Cell *cell);
    int isConnectTo(Cell *cell);
	int isConnectTo(vector<Cell *>);
	int isConnectTo(CellSet *cellset);
    bool isOriginal();
	int setOriginal(int ori);
    int getValue();
	int getMarker(){ return _mark; }
    const SPoint &getPoint() const;
    int reset();
    int setValue(int num,int mark=CBLACK, bool bReset=true);
    const char *getName();
	int connectCellSet(EnumSetType tp,CellSet *cs);
};

class CellSet
{
public:
	CellSet(EnumSetType tp);
private:
    vector<Cell *> _cells;
	char _name[100];
    EnumSetType _type;
	int _id;

public:
    int addCell(Cell *cell);
	int hasCell(Cell *cell);
    vector<Cell *> getCells();
	EnumSetType getType() ;
	int getID();
	const char* getName();
};

class Puzzle
{
public:
    Puzzle();
    ~Puzzle();
private:
    Puzzle(const Puzzle &use){};
private:
	string _strOriginal;
    vector<Cell *> _cells;
    vector<CellSet*> _rows;
    vector<CellSet*> _cols;
    vector<CellSet*> _boxs;
    vector<CellSet*> _sets; //convience for set: _rows, _cols and _boxs
public:
    int loads(string str,string strcur="",string strallcands=""); //Load puzzle from string
    string dumps(int tp=FMT_LINE);
	string dumps_original(int tp=FMT_LINE);
	string getOriginal();
    int isDone(); // Is this puzzle solved ?
    Cell *getCell(int cx,int cy);
	Cell *getCell(SPoint &pt) {return getCell(pt.getx(),pt.gety());}
	vector<Cell *>& getCells();
	vector<CellSet *>& getCellSetList();
    CellSet *getCellSet(int tp,int cc);
    int copyFrom(Puzzle &pz);
};

// Use a JSON or XML string to save an SAction object
class Operation
{
public:
	Operation(EnumSActionType _tp=ACT_SET):tp(_tp),st(ST_NONE),color(CBLACK){}
	Operation(EnumSActionType _tp, SPoint pt, int num, EnumStatus _st = ST_NONE, EnumColor _color = CBLACK) :tp(_tp), st(_st), color(_color)
	{
		pts.push_back(pt);
		nums.push_back(num);
	}
	EnumSActionType tp;
	vector<SPoint> pts;
	vector<int> nums;
	EnumStatus st; // not for all
	EnumStatus oldst; // not for all
	EnumColor color;
	vector<vector<int> > rcands;
	void loads(const string & opstr);
	string dumps();
};

class SAction
{
public:
	SAction(EnumSolver s):nSolver(s){}
	SAction(EnumSolver s,Operation op):nSolver(s){ops.push_back(op);}
	
	EnumSolver nSolver;
	vector<Operation> ops;
	
	string dumps();

	//because: cellsets, cells/SPoints, numbers
	//operation: cells/SPoints, numbers,actype
	vector<CellSet *> cellsets;
	vector<int> nums;
	vector<SPoint> pts;
	vector< vector<int> > chain;
};

class SNode
{
public:
	int _x;
	int _y;
	int _n;
	vector< vector<int> > _stronglink;
	vector< vector<int> > _weaklink;

	SNode():
	  _x(-1),_y(-1),_n(-1)
	  {

	  }
	SNode(int x,int y,int n):
	  _x(x),_y(y),_n(n)
	  {

	  }
	SNode(SPoint &pt, int n):
	  _x(pt.getx()),_y(pt.gety()),_n(n)
	  {

	  }
    
    bool operator ==(const SNode& node)
	{
		return node._x == _x && node._y == _y  &&  node._n == _n;
	}
	bool operator ==(const vector<int> &linknode)
	{
		if(linknode.size()!=3) return false;
		return linknode[0] == _x && linknode[1] == _y  &&  linknode[2] == _n;
	}

	bool isValid(){
		return _x != -1 && _y != -1 && _n!=-1;
	}
	bool link(SNode &node,bool isStrongLink);
	bool link(int x,int y,int cand,bool isStrongLink);
};

const char* toSolverName(EnumSolver solver);

class Solver
{
private:
	Puzzle _pz;
	vector<SAction> _acts;
	static vector<vector<vector<int> > > vvv;
	string _szResult;
	SNode _nodes[NUM][NUM][NUM];
	int m_curNum;
public:
	Solver();
	~Solver();

	void setCurrentNumber(int n) { m_curNum = n; }
	int *randIDX();
	const char*getSolverName(EnumSolver solver);

	int executeSActions(Puzzle &pz,vector<SAction> &acts);
	void resetNodes();
	void updateNodes();
	void updateNodes(int x, int y);
	int findChain(EnumSolver solver, vector<SAction> &acts, vector< vector<int> > &chain,int r,int x, int y, int cand, bool bStrongLink);
	int shortChain(vector< vector<int> > &chain);

	vector<SAction> doSolve(Puzzle *pz,int step=1);
	int tryGuess(Puzzle &pz,vector<SAction>&acts);
	int trySingleNumber(Puzzle &pz,vector<SAction> &acts);
	int tryIntersectionRemoval(Puzzle &pz,vector<SAction> &acts);
	int tryHiddenNumberSet(Puzzle &pz,vector<SAction> &acts,int nDim);
	int tryNakedNumberSet(Puzzle &pz,vector<SAction> &acts,int nDim);
	int tryXYWing(Puzzle &pz,vector<SAction> &acts); //XY-Wing(Y-Wing)
	int tryXYZWing(Puzzle &pz,vector<SAction> &acts); //XYZ-Wing
	int tryUniqueRectangle(Puzzle &pz,vector<SAction> &acts);
	int tryFish(Puzzle &pz,vector<SAction> &acts,int nDim); //X-Wing, Sword-Fish, Jelly-Fish
	int tryBug1(Puzzle &pz,vector<SAction> &acts);
	int tryXChain(Puzzle &pz,vector<SAction> &acts);
	int tryXYChain(Puzzle &pz,vector<SAction> &acts);
	int tryForcingChain(Puzzle &pz,vector<SAction> &acts);
};

SPoint xy2rc(const SPoint &xy);
SPoint rc2xy(const SPoint &rc);
SPoint id2xy(int id);

int LIBAPI cord2num(int cord);


#endif

