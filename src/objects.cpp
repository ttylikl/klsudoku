#include "common.h"
#include "funcs_temp.h"

#include "ccutils.h"

const char* g_stname[] = { "blue", "black", "tag", "green", "red", "brown" };

void Operation::loads(const string & opstr){
	rapidjson::Document d;
	d.Parse<0>(opstr.c_str());
	if (d.HasParseError()){
		plog("%s -> GetParseError: %s", __FUNCTION__, d.GetParseError());
	}
	else if (d.IsObject()){
		if (!d.HasMember("act") || !d.HasMember("st") || !d.HasMember("color") || !d.HasMember("nums") || !d.HasMember("pts") || !d.HasMember("rcands"))
		{
			plog("%s : some member key is not exist!", __FUNCTION__);
			return;
		}
		tp = (EnumSActionType)d["act"].GetInt();
		st = (EnumStatus)d["st"].GetInt();
		oldst = (EnumStatus)d["oldst"].GetInt();
		color = (EnumColor)d["color"].GetInt();
		nums.clear();
		pts.clear();
		rcands.clear();
		//example: for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr) printf("%d ", itr->GetInt());
		if (d["nums"].IsArray()){
			for (unsigned int i = 0; i < d["nums"].Size(); i++)
			{
				nums.push_back(d["nums"][i].GetInt());
			}
		}
		if (d["pts"].IsArray()){
			for (unsigned int i = 0; i < d["pts"].Size(); i++)
			{
				string strpt = d["pts"][i].GetString();
				SPoint pt(strpt.c_str());
				pts.push_back(pt);
			}
		}
		if (d["rcands"].IsArray()){
			for (unsigned int i = 0; i < d["rcands"].Size(); i++)
			{
				string strtmp = d["rcands"][i].GetString();
				vector<string> strlist;
				vector<int> _rcs;
				Utils::split(strtmp, strlist, ",");
				if (strlist.size() != 4)
				{
					plog("Wrong rcands item:%s", strtmp.c_str());
					continue;
				}
				for (unsigned int j = 0; j < strlist.size(); j++){
					_rcs.push_back(atoi(strlist.at(j).c_str()));
				}
				rcands.push_back(_rcs);
			}
		}
	}
}
string Operation::dumps(){
	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType &allocator = d.GetAllocator();
	rapidjson::Value _nums(rapidjson::kArrayType);
	for (unsigned int i = 0; i < nums.size(); i++)
	{
		_nums.PushBack(nums.at(i), allocator);
	}
	rapidjson::Value _pts(rapidjson::kArrayType);
	for (unsigned int i = 0; i < pts.size(); i++)
	{
        rapidjson::Value _name(rapidjson::kStringType);
        const char *_sz = pts.at(i).getName();
        _name.SetString(_sz, strlen(_sz));
		_pts.PushBack(_name, allocator);
	}
	rapidjson::Value _rcands(rapidjson::kArrayType);
	for (unsigned int i = 0; i < rcands.size(); i++)
	{
		char buf[100];
		vector<int> _rcs;
		_rcs=rcands.at(i);
		if (_rcs.size() != 4){
			plog("Bad relevant cands, skipped!%d", _rcs.size());
			continue;
		}
		sprintf(buf, "%d,%d,%d,%d", _rcs.at(0), _rcs.at(1), _rcs.at(2), _rcs.at(3));
		rapidjson::Value sz(rapidjson::kStringType);
		sz.SetString(buf,allocator);
		_rcands.PushBack(sz, allocator);
		//_rcands.PushBack(buf, allocator);
	}

	d.AddMember("act", tp, allocator);
	d.AddMember("st", st, allocator);
	d.AddMember("oldst", st, allocator);
	d.AddMember("color", color, allocator);
	d.AddMember("nums", _nums, allocator);
	d.AddMember("pts", _pts, allocator);
	d.AddMember("rcands", _rcands, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	return buffer.GetString();
}


static std::string& implode(const std::vector<std::string>& elems, char delim, std::string& s)
{
    for (std::vector<std::string>::const_iterator ii = elems.begin(); ii != elems.end(); ++ii)
    {
        s += (*ii);
        if ( ii + 1 != elems.end() ) {
            s += delim;
        }
    }

    return s;
}

static std::string implode(const std::vector<std::string>& elems, char delim)
{
    std::string s;
    return implode(elems, delim, s);
}

static vector<string> toStringVector(const vector<int>& elems) {
	vector<string> vs;
	char nbuf[100];
	for (std::vector<int>::const_iterator ii = elems.begin(); ii != elems.end(); ++ii){
		sprintf(nbuf, "%d", *ii);
		vs.push_back(nbuf);
	}
	return vs;
}

static vector<string> toStringVector(const vector<SPoint>& elems) {
	vector<string> vs;
	for (std::vector<SPoint>::const_iterator ii = elems.begin(); ii != elems.end(); ++ii){
		SPoint pt = *ii;
		string s = pt.dumps();
		vs.push_back(s);
	}
	return vs;
}
static vector<string> toStringVector(const vector<Operation>& elems) {
	vector<string> vs;
	for (std::vector<Operation>::const_iterator ii = elems.begin(); ii != elems.end(); ++ii){
		Operation pt = *ii;
		string s = pt.dumps();
		vs.push_back(s);
	}
	return vs;
}

static vector<string> toStringVector(const vector<CellSet *>& elems) {
	vector<string> vs;
	vector<CellSet *>::const_iterator it=elems.begin();
	while(it!=elems.end())
	{
		CellSet *cellset = *it++;
		char tmp[32];
		char buf[100];
		sprintf(tmp,"<%s>", cellset->getName());
		sprintf(buf,"{\"name\":\"%s\", \"type\":%d, \"id\":%d}", tmp, cellset->getType(), cellset->getID());
        vs.push_back(buf);
	}
	return vs;
}

static string toString(int n) {
	char buf[32];
	sprintf(buf,"%d", n);
	return buf;
}
/*

	//because: cellsets, cells/SPoints, numbers
	//operation: cells/SPoints, numbers,actype
	vector<CellSet *> cellsets;
	vector<int> nums;
	vector<SPoint> pts;
	vector< vector<int> > chain;
*/

string SAction::dumps() {
	// string numstr = implode(toStringVector(nums), ',');
	// numstr = string("[").append(numstr).append("]");
	// string ptstr = implode(toStringVector(pts),',');
	vector<string> chains;
	for(int i=0;i<chain.size();i++){
		vector<int> vi = chain.at(0);
		string s = implode(toStringVector(vi), ',');
		chains.push_back(s);
	}
	// printf("%d numstr: %s\n", nums.size(), numstr.c_str());
	// printf("%d ptstr: %s\n", pts.size(), ptstr.c_str());
	// printf("chainsize:%d\n", chain.size());
	// printf("%d chains:%s\n", chain.size(), implode(chains, ',').c_str());
	// printf("cellsets size:%d dumps:%s\n", cellsets.size(), implode(toStringVector(cellsets), ',').c_str());
	// printf("operations size:%d operation: %s\n", ops.size(), implode(toStringVector(ops), ',').c_str());
	string ret="{";
	ret.append("\"solver\":").append(toString(nSolver)).append(","); // {"solver":0,
	ret.append("\"solvername\":\"").append(toSolverName(nSolver)).append("\","); // {"solver":0,
	ret.append("\"ops\":[").append(implode(toStringVector(ops), ',')).append("],");
	ret.append("\"nums\":[").append(implode(toStringVector(nums), ',')).append("],");
	ret.append("\"pts\":[").append(implode(toStringVector(pts), ',')).append("],");
	ret.append("\"cellsets\":[").append(implode(toStringVector(cellsets), ',')).append("],");
	ret.append("\"chains\":[").append(implode(chains, ',')).append("]");
	ret.append("}");
	return ret;
}

const char*toSolverName(EnumSolver solver)
{
	/*
	enum EnumSolver { S_Manual, S_Guess, 
	S_NakedSingleNumber, S_HiddenSingleNumber, 
	S_IntersectionRemoval, 
	S_HiddenPair, S_HiddenTriplet, S_HiddenQuad, 
	S_NakedPair, S_NakedTriplet, S_NakedQuad,
	S_XWing, S_XYWing, S_XYZWing,
	S_SwordFish, S_JellyFish,
	S_WWing,
	S_UniqueRectangle1, S_UniqueRectangle2, S_UniqueRectangle3, S_UniqueRectangle4, S_UniqueRectangle5, S_UniqueRectangle6, S_UniqueRectangle7, 
	S_XChain, S_XYChain, S_ForcingChain, 
	S_BUG1
	};
	*/
	switch(solver)
	{
	case S_Guess: return "Guess";
	case S_Manual: return "Manual";
	case S_HiddenSingleNumber: return "HiddenSingleNumber";
	case S_NakedSingleNumber: return "NakedSingleNumber";
	case S_IntersectionRemoval: return "IntersectionRemoval";
	case S_HiddenPair: return "HiddenPair";
	case S_HiddenTriplet: return "HiddenTriplet";
	case S_HiddenQuad: return "HiddenQuad";
	case S_NakedPair: return "NakedPair";
	case S_NakedTriplet: return "NakedTriplet";
	case S_NakedQuad: return "NakedQuad";
	case S_XWing: return "X-Wing";
	case S_XYWing: return "XY-Wing";
	case S_XYZWing: return "XYZ-Wing";
	case S_SwordFish: return "SwordFish";
	case S_JellyFish: return "JellyFish";
	case S_WWing: return "W-Wing";
	case S_UniqueRectangle1: return "UniqueRectangle1";
	case S_UniqueRectangle2: return "UniqueRectangle2";
	case S_UniqueRectangle3: return "UniqueRectangle3";
	case S_UniqueRectangle4: return "UniqueRectangle4";
	case S_UniqueRectangle5: return "UniqueRectangle5";
	case S_UniqueRectangle6: return "UniqueRectangle6";
	case S_UniqueRectangle7: return "UniqueRectangle7";
	case S_XChain: return "X-Chain";
	case S_XYChain: return "XY-Chain";
	case S_ForcingChain: return "Forcing-Chain";
	case S_BUG1: return "Bug+1";
	default: return "Unknown";
	}
}

Cell::Cell(int cx,int cy)
    : _xy(cx,cy),_mark(CBLACK),_bDirty(1)
{
	*_name = 0;
	memset(_cellset,0,sizeof(_cellset));
    reset();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    getName(); //update name data
#endif
    
}

int Cell::reset()
{
    _num=NUM_NONE;
    for(int i=0;i<NUM;i++)
    {
        _cands[i]=ST_NORMAL;
    }
    return _num;
}


int Cell::copyFrom(Cell*sc)
{
    //copy data member to this, do not reference!
    _num=sc->_num;
	_mark = sc->_mark;
    _xy.setxy(sc->_xy.getx(),sc->_xy.gety());
    memcpy(_cands,sc->_cands,sizeof(_cands));
    memcpy(_name,sc->_name,sizeof(_name));
    //NOTICE: _vcells can't be copied, because it should be set with method 'this->connect()' by container which is instance of class Puzzle
    return 0;
}

vector<Cell*>& Cell::getConnectCells()
{
    return _vcells;
}

int Cell::setOriginal(int ori)
{
	int ret=_mark;
	if (ori)
		_mark = CBLACKBOLD;
	else
		_mark = CBLACK;
	return ret;
}

int Cell::setValue(int num, int mark, bool bReset)
{
	int oldnum=_num;
	int rc=0;
	if (isOriginal())// _mark == CBLACK )
    {
        plog("Can't set the value[%d] of an original cell:%s!", num, getName());
        return -2;
    }
    if(NUM_NONE == num)
    {
		_bDirty=1;

		if (bReset) //do not reset when we make undo/redo 20140924
		{
			reset();
		}
		else
			_num = NUM_NONE;

        //Unset Candidates for cells which connected to this
	    
        for(unsigned int i=0;i<_vcells.size();i++)
	    {
	        Cell *ct=_vcells[i];
	        if(NUM_NONE != oldnum)
			{
				//check the value of the cell which connect with 'ct'
				int bSetCT=1;
				vector<Cell *> ctcts=ct->getConnectCells();
				for(unsigned int j=0;j<ctcts.size();++j)
				{
					if( ctcts[j]->getValue() == oldnum)
					{
						bSetCT=0;
						break;
					}
				}
				if(bSetCT)
					ct->setCandStatus(oldnum,ST_NORMAL);//TODO: we should return to last candidate status, but a list ? not necessary!
			}
			int n=ct->getValue();
	        if(n!=NUM_NONE)
	        {
	            setCandStatus(n,ST_NONE);
	        }
	    }
    	return rc;
    }
    if(!isValidNum(num))
    {
		plog("Try to set invalid number(%d) to Cell:%s", num, getName());
        return -1;
    }
	_bDirty=1;
    //check connect cells. we do not use candidates now.
    for(unsigned int i=0;i<_vcells.size();i++)
    {
        Cell *cell=_vcells[i];
        if(cell->getValue() == num)
        {
            //return -3; // A number can't set twice in same cellset, but we should not return here, because player maybe make mistake!
            plog("Player has made some mistake, %s can not be %d because %s", getName(), num, cell->getName());
			rc=-3;
            break;
        }
    }
	_num = num;
	_mark = mark;
	if (_mark <0 || _mark>CMAX)
	{
		plog("Bug! Wrong mark value[%d]", _mark);
		//throw "Wrong Mark";
		_mark = 0;
	}
	*_name = 0; //force to regenerate name string
    //plog("%s::setValue(%d)",this->getName(),num);
    for(unsigned int i=0;i<_vcells.size();i++)
    {
        _vcells[i]->setCandStatus(num,ST_NONE);
		//plog("set cand[%d] to ST_NONE for %s",num,_vcells[i]->getName());
    }
    return rc;
}

EnumStatus Cell::setCandStatus(int cand,EnumStatus st)
{
    if(!isValidNum(cand))
    {
        plog("ERROR: try to set wrong candidate status for [%d]",cand);
        return ST_NONE;
    }
    //if(_xy.equal(0,1) || _xy.equal(1,0) )
    //    plog("set cand[%d] of %s to %d",cand,_xy.getName(),st);
	_bDirty=1;
    if(st!=ST_NONE)
    {
        for(unsigned int i=0;i<_vcells.size();i++)
        {
            if(_vcells[i]->getValue()==cand)
            {
                //plog("We can't set candidate[%d] for %s, because %s", cand, getName(), _vcells[i]->getName() );
                return ST_NONE;
            }
        }
    }
	//plog("cand=%d", cand);
    int idx=num2cord(cand);
    EnumStatus ret=_cands[idx];
    _cands[idx]=st;
    return ret;
}

EnumStatus Cell::getCandStatus(int cand)
{
	//plog("cand=%d", cand);
    int cc=num2cord(cand);
    if(isValidCord(cc) && _num==NUM_NONE)
    {
        return _cands[cc];
    }
    return ST_NONE;
}

vector<int>&Cell::getCands(EnumStatus st)
{
	if(!_bDirty && st==ST_NORMAL)
		return _vcands;
    vector<int> ret;
    for(int cc=0;cc<NUM && _num==NUM_NONE;cc++)
    {
        if(_cands[cc] == st)
        {
            ret.push_back(cord2num(cc));
        }
    }
	if(st==ST_NORMAL)
	{
		_vcands=ret;
		_bDirty=0;
	}
    return _vcands;
}

int Cell::getValue()
{
    return _num;
}

const SPoint &Cell::getPoint() const
{
    return _xy;
}

const char * Cell::getName()
{
	if (*_name == 0)
	{
		SPoint rc = xy2rc(_xy);
		sprintf(_name, "R%dC%d[%d]", rc.getx(), rc.gety(), _num);
	}
	return _name;
}


int Cell::connect(Cell *cell)
{
    if(isConnectTo(cell))
        return 0;
    _vcells.push_back(cell);
    cell->connect(this);
	_bDirty=1;
    return _vcells.size();
}

int Cell::isConnectTo(Cell *cell)
{
    for(unsigned int i=0;i<_vcells.size();i++)
    {
        //Cell *ct=_vcells[i];
        //if(cell->getPoint().equal(ct->getPoint()) ) return 1
		if(_vcells[i]==cell) // we are in same puzzle & we had only one instance ! so we can compare with address! more effective
            return 1;
    }
    return 0;
}

int Cell::isConnectTo(CellSet *cellset)
{
	int tp=cellset->getType();
	return (_cellset[tp]==cellset);	
	//return _cellset[tp]->getID() = cellset->getID();
}

int Cell::isConnectTo(vector<Cell *> cells)
{
	for(unsigned int ci=0;ci<cells.size();++ci)
	{
		if(!isConnectTo(cells[ci]))
			return 0;
	}
	return 1;
}

int Cell::connectCellSet(EnumSetType tp,CellSet *cs)
{
	_cellset[tp]=cs;
	_bDirty=1;
	return 0;
}

CellSet * Cell::getConnectCellset(EnumSetType tp)
{
	return _cellset[tp];
}


bool Cell::isOriginal()
{
    return _mark==CBLACKBOLD;
}

CellSet::CellSet(EnumSetType tp)
    : _type(tp),_id(0)
{
	memset(_name,0,sizeof(_name));
}

int CellSet::hasCell(Cell *cell)
{
	SPoint pt=cell->getPoint();
	switch(_type)
	{
	case BOX: return _id==cord2boxid(pt); 
	case COL: return _id==pt.gety();
	case ROW: return _id==pt.getx();
	}
	return 0;
}

int CellSet::addCell(Cell *cell)
{
	if(!_id)
	{
		SPoint pt=cell->getPoint();
		switch(_type)
		{
		case BOX:
			_id=cord2boxid(pt.getx(),pt.gety()); 
			break;
		case COL:
			_id=pt.gety();
			break;
		case ROW:
			_id=pt.getx();
			break;
		}
	}
    if (_cells.size() > NUM)
    {
        plog("addCell too much cells to a CellSet");
        return -1;
    }
    for(unsigned int i=0;i<_cells.size();i++)
    {
        _cells[i]->connect(cell);
    }
    _cells.push_back(cell);
	cell->connectCellSet(_type,this);
    return _cells.size();
}

vector<Cell*> CellSet::getCells()
{
    return _cells;
}

EnumSetType CellSet::getType() 
{ 
	return _type;
}

int CellSet::getID() 
{
	return _id;
}

const char * CellSet::getName()
{
	if(*_name!=0)
		return _name;
	if(_cells.size()==0)
	{
		switch(_type)
		{
		case BOX: return "BOX";
		case ROW: return "ROW";
		case COL: return "COL";
		default:
			plog("Wrong SetType for CellSet!");
			return "ERR";
		}
	}
	SPoint pt=_cells[0]->getPoint();
	switch(_type)
	{
	case BOX:
	{
		//int rc_boxid = ((8 - pt.gety()) / BOXNUM) * BOXNUM + (pt.getx() / BOXNUM) + 1;
        int rc_boxid = ((pt.gety()) / BOXNUM) * BOXNUM + (pt.getx() / BOXNUM) + 1;
        sprintf(_name, "BOX%d", rc_boxid);
		break;
	}
	//case COL: sprintf(_name,"ROW%d",9-pt.gety());break;
    case COL: sprintf(_name,"ROW%d",pt.gety()+1);break;
	case ROW: sprintf(_name,"COL%d",pt.getx()+1);break;
	default:
		plog("Wrong SetType for CellSet!");
		return "ERR";
	}
	return _name;
}

Puzzle::Puzzle()
{
	for (unsigned int cb = 0; cb < NUM; cb++)
	{
		CellSet *cellset = new CellSet(BOX);
		_boxs.push_back(cellset);
		_sets.push_back(cellset);
	}
    for(unsigned int cx=0;cx < NUMROW;cx++)
    {
        CellSet *cellset=new CellSet(ROW);
        _rows.push_back(cellset);
        _sets.push_back(cellset);
    }
    for(unsigned int cy=0;cy < NUMROW;cy++)
    {
        CellSet *cellset=new CellSet(COL);
        _cols.push_back(cellset);
        _sets.push_back(cellset);
    }
    for(unsigned int cx=0;cx < NUMROW;cx++)
    {
        for(unsigned int cy=0;cy < NUMCOL;cy++)
        {
            int cb=cord2boxid(cx,cy);
            Cell *cell=new Cell(cx,cy);
            _cells.push_back(cell);
            //plog("Create %d cell(%d,%d), push it into rows[%d] cols[%d] and boxs[%d] ",_cells.size(), cx,cy,cx,cy,cb);
            _rows[cx]->addCell(cell);
            _cols[cy]->addCell(cell);
            _boxs[cb]->addCell(cell);
        }
    }
}

Puzzle::~Puzzle()
{
    for(unsigned int i=0;i<_cells.size();i++)
    {
        free(_cells[i]);
    }
    for(unsigned int i=0;i<_sets.size();i++)
    {
        free(_sets[i]);
    }
}

int Puzzle::copyFrom(Puzzle &pz)
{
    for(unsigned int cx=0;cx < NUMROW;cx++)
    {
        for(unsigned int cy=0;cy < NUMCOL;cy++)
        {
            getCell(cx,cy)->copyFrom(pz.getCell(cx,cy));
        }
    }
    return 0;
}

string Puzzle::getOriginal()
{
	return _strOriginal;
}

#define PLOG if(false) plog

int Puzzle::loads(string str,string scur, string strallcands)
{
    const char *sz=str.c_str();
    const char *szc=scur.c_str();
    int idx=0;
	int lsc=(int)strlen(szc);
	_strOriginal=str;
	//garbage code, comment this
    for(unsigned int i=0;i<_cells.size();i++)
	{
        _cells[i]->reset();
		_cells[i]->setOriginal(0);
	}
	// load all candidates status
	PLOG("load all candidates!");
	char candstr[900]="";
	strcpy(candstr, strallcands.c_str());
	char * semi = candstr;
	vector<vector<int>> allcands;
	for(unsigned int i=0;i<81;i++){
		char *tail = strchr(semi, ';');
		if(tail){
			*tail=0;
		}
		vector<int> cands;
		for(unsigned int j=0;j<strlen(semi);j++){
			if(semi[j] == ',') continue; // skip comma
			unsigned int cand = semi[j] - '1' + 1;
			cands.push_back(cand);
		}
		allcands.push_back(cands);
		if(!tail) {
			// no more semicolon !
			break;
		}
		semi= tail +1;
	}
	// for(unsigned int i=0;i<allcands.size();i++){
	// 	vector<int> cands = allcands.at(i);
	// 	for(unsigned int j=0;j<cands.size();j++){
	// 		printf("%d", cands.at(j));
	// 	}
	// 	printf("\n");
	// }
	PLOG("load all candidates over");

	//20140910
	//for (int cx = NUMROW-1; cx >=0; cx--)

	//for new board version! 20141224
	for (int cx = 0; cx<NUMROW; cx++)
	{
		vector<Cell*> cells = _cols[cx]->getCells();
		for (int cy = 0; cy<NUMCOL; cy++)
        {
			Cell *cell=cells[cy];
			//cell->setOriginal(0); //unseal&clear the cell 
			char ch;
			char chc;
            if(idx!=-1)
            {
                do
                {
                    ch=*(sz+idx);
					chc=*(szc+idx);
                    idx++;
                    if(!(ch=='\r' || ch=='\n' || ch=='+' || ch == '|' || ch=='-'))
                        break;
                }while(1); //skip some character
                if(ch==0)
                {
                    idx=-1;
                }
                else
                {
                    if(ch>='1' && ch <= '9')
					{
						cell->setValue(ch-'0');//set isOriginal to non-zero
						cell->setOriginal(1); //seal the cell 
						//plog("\n%s",dumps(FMT_CELL_CANDS).c_str());
					}
					else
                    {
						cell->setValue(NUM_NONE);//
						if(lsc>=idx)
						{
							if(chc>='1' && chc <= '9')
								cell->setValue(chc-'0');
						}
                    }
                }
            }
			else
            {
				cell->setValue(NUM_NONE);//
				if(lsc>=idx)
				{
					ch=*(szc+idx);
					if(ch>='1' && ch <= '9')
						cell->setValue(ch-'0');
				}
            }
        }
    }
	if(*szc==0)
		_strOriginal=dumps(FMT_LINE);
	
	PLOG("set puzzle candidates status base on all candidates vector");
	for (int cx = 0; cx<NUMROW; cx++)
	{
		vector<Cell*> cells = _cols[cx]->getCells();
		for (int cy = 0; cy<NUMCOL; cy++)
        {
			PLOG("cx,cy = %d, %d", cx, cy);
			Cell *cell=cells[cy];
			int idx = cx*9 + cy;
			PLOG("idx=%d allcands.size=%d", idx, allcands.size());
			vector<int> cands = allcands.at(idx);
			for(int cand=1; cand <=9; cand++) {
				int st = cell->getCandStatus(cand);
				if(st == ST_NORMAL) {
					vector<int>::iterator fi=find(cands.begin(),cands.end(),cand);
					if(fi == cands.end()){
						cell->setCandStatus(cand, ST_TAG);
					}
				}
			}
		}
	}
	printf("loads done!\n");
    return 0;
}

string Puzzle::dumps_original(int tp)
{
    if(FMT_LINE==tp)
        return _strOriginal;
    Puzzle pz;
    pz.loads(_strOriginal);
    return pz.dumps(tp);
}

string Puzzle::dumps(int tp)
{
    //char buf[100*100 /*NUMROW*NUMCOL*NUM*NUM*/ ]={0};
    int bufsize=100*100;
    char *buf=(char*)malloc(bufsize);
    memset(buf,0,bufsize);
    int idx=0;
    switch(tp)
    {
        case FMT_CELL_CANDS:
        case FMT_CELL:
		{
				//for (int cy = NUMROW - 1; cy >= 0; cy--)
				for (int cy = 0; cy < NUMROW; cy++)
				{
					vector<Cell*> cells = _cols[cy]->getCells();
					for (int cx = 0; cx<NUMCOL; cx++)
					{
						Cell *cell=cells[cx];
						if(!cell->getPoint().equal(cx,cy))
						{
							plog("WRONG!");
						}

						int val=cell->getValue();
                        char tmp[16];
                        if  (val  != 0)
                        {
                            sprintf(tmp,"[____%d____]",val);
                        }
                        else
                        {
                            if(tp==FMT_CELL_CANDS)
                            {
                                vector<int> cands=cell->getCands(ST_NORMAL);
                                char csz[NUM+1];
                                memset(csz,0,sizeof(csz));
                                for(int cc=0;cc<(int)cands.size();cc++)
                                {
                                    *(csz+cc)=(char)(cands[cc]+'0');
                                }
                                sprintf(tmp,"[%9s]",csz);
                            }
                            else
                            {
                                strcpy(tmp,"[_________]");
                            }
                        }
                        strcat(buf,tmp);
                        idx+=strlen(tmp);
						if((cx+1)%BOXNUM==0)
                        {
                            buf[idx++] = ' ';
                            buf[idx++] = ' ';
                            buf[idx++] = ' ';
                            buf[idx++] = ' ';
                        }
                    }
                    buf[idx++] = '\n';

                    if((cy)%BOXNUM==0)
                        buf[idx++] = '\n';
                }
            }
            break;
        case FMT_LINE:
        case FMT_LF:
        default:
			//for (int cy = NUMROW - 1; cy >= 0; cy--)
			for (int cy = 0; cy < NUMROW; cy++)
			{
				vector<Cell*> cells = _cols[cy]->getCells();
				for (int cx = 0; cx<NUMCOL; cx++)
                {
                    Cell *cell=cells[cx];
                    if(!cell->getPoint().equal(cx,cy))
                    {
                        plog("WRONG!");
                    }
                    int val=cell->getValue();
                    if  (val  != 0)
                    {
                        buf[idx++]='1'+val-1;
                    }
					else
					{
                        //buf[idx++]='.';
						buf[idx++] = '0';
					}
                }
                if(tp!=FMT_LINE)
                    buf[idx++] = '\n';
            }
    }
    string str=string(buf);
    free(buf);
    return str;
}

vector<Cell *>& Puzzle::getCells()
{
	return _cells;
}

Cell *Puzzle::getCell(int cx,int cy)
{
    int cc=cx*NUMROW + cy;
    if(cc >= NUMROW*NUMCOL)
    {
        plog("Wrong cell cord, %d >= %d",cc,NUMROW*NUMCOL);
        return NULL;
    }
    Cell *cell=_cells[cc];
    SPoint xy=cell->getPoint();
    if(!xy.equal(cx,cy))
    {
        plog("ERROR: getCell() get wrong cordinate[%s : %d,%d]",xy.getName(),cx,cy);
    }
    return cell;
}

vector<CellSet *>& Puzzle::getCellSetList()
{
	return _sets;
}

CellSet *Puzzle::getCellSet(int tp,int cc)
{
    if(!isValidCord(cc))
    {
        plog("Puzzle::getCellSet() with wrong cc[%d]",cc);
        return NULL;
    }
    switch(tp)
    {
        case ROW:
            return _rows[cc];
        case COL:
            return _cols[cc];
        case BOX:
            return _boxs[cc];
        //default:
    }
    plog("Puzzle::getCellSet() with wrong SetType[%d]",tp);
    return NULL;
}

int Puzzle::isDone()
{
    int tags[NUM]={0};
    for(unsigned int i=0;i<_cells.size();i++)
    {
        int n=_cells[i]->getValue();
        if(NUM_NONE==n) return 0;
		// plog("cand=%d", n);
        tags[num2cord(n)]++;
    }
    for(unsigned int j=0;j<NUM;j++)
    {
        if(tags[j]!=9) return 0;
    }
    return 1;
}


bool SNode::link(SNode &node,bool isStrongLink)
{
	return link(node._x,node._y,node._n,isStrongLink);
}

bool SNode::link(int x,int y, int cand,bool isStrongLink)
{
	//vector<int> linknode;
	//linknode.push_back(x);
	//linknode.push_back(y);
	//linknode.push_back(cand);
	if(isStrongLink)
	{
		for (unsigned int i = 0; i < _stronglink.size(); ++i)
		{
			//if (vv_equal(_stronglink[i], linknode))
			if (
				_stronglink[i][0] == x &&
				_stronglink[i][1] == y &&
				_stronglink[i][2] == cand
				)
				return false;
		}
		vector<int> linknode(3, 0);
		linknode[0] = x;
		linknode[1] = y;
		linknode[2] = cand;
		_stronglink.push_back(linknode);
	}
	else
	{
		for (unsigned int i = 0; i < _weaklink.size(); ++i)
		{
			//if (vv_equal(_weaklink[i], linknode))
			if (
				_weaklink[i][0] == x &&
				_weaklink[i][1] == y &&
				_weaklink[i][2] == cand
				)
				return false;
		}
		vector<int> linknode(3, 0);
		linknode[0] = x;
		linknode[1] = y;
		linknode[2] = cand;
		_weaklink.push_back(linknode);
	}
	return true;
}


SPoint xy2rc(const SPoint &xy)
{
	//return SPoint(9 - xy.gety(), xy.getx() + 1
	//20141224 recode !
	return SPoint( xy.gety()+1, xy.getx() + 1);
}

SPoint rc2xy(const SPoint &rc)
{
	//20141224 recode !
	return SPoint( rc.gety()-1, rc.getx() - 1);
}

SPoint id2xy(int id)
{
	return SPoint(id%9, 8-id/9);
}
