using std;
using wx;
using #sparrowlib;

spw  = sparrowlib::sparrow();
assert(spw);
nl = "\n";
const classId = "DependencyGraph";

std::dllimportdeltalib(wx::DLL);	//do not unload explicitly
initallimagehandlers();

Renderer		= 
Geometry		= 
Data			=
fileGeometry	=
currGeometry	= 
projectGeometry = nil;

window	= nil;

function NewNode (file,Nodes)
{	
	local tmp =
	[
		@name		: 0	,
		@x			: 0	,
		@y			: 0	,
		@width		: 0	,
		@height		: 0	,
		@BGcolour	: "Node_BGcolour" ,
		@Level		: 0			,
		@inLines	: list_new(),
		@outLines	: list_new(),
		@BorderWidth	: "DefaultBorderWidth",
		@Shape_Colour: "DefaultShapeColour",
		@file 	: file,

		method (name,x,y,width,height)
		{
			@name	= name;
			@x		= x;
			@y		= y;
			@width	= width;
			@height	= height;
		},
		method Reset()
		{
			@BorderWidth	= "DefaultBorderWidth";
			
			@Shape_Colour	= "DefaultShapeColour";
			foreach(local l,@inLines)
			{
				l.from.Shape_Colour	= "DefaultShapeColour";
				l.from.BorderWidth	= "DefaultBorderWidth";
				l.Reset();
			}
			foreach(local l,@outLines)
			{
				l.to.Shape_Colour	= "DefaultShapeColour";
				l.to.BorderWidth	= "DefaultBorderWidth";
				l.Reset();
			}
			
		},
		method Move(dx,dy)
		{
			@x	+= dx;
			@y	+= dy;
		},
		method StopMoving()
		{
			@Reconnect();
		},
		method Highlight()
		{
			@BorderWidth	= "SelectedBorderWidth";
			@Shape_Colour	= "SelectedNodeColour";
			local inColour	= "Selected_incoming_colour";
			foreach(line,@inLines)
			{
				line.colour				= 
				line.from.Shape_Colour	= inColour;
				line.width				= "SelectedLineWidth";
				line.from.BorderWidth	= "SelectedBorderWidth";
			}
				
			local outColour	= "Selected_outgoing_colour";
			foreach(line,@outLines)
			{
				line.colour				= 
				line.to.Shape_Colour	= outColour;
				line.width				= "SelectedLineWidth";
				line.to.BorderWidth		= "SelectedBorderWidth";
			}

		},
		
		method SetLevel(l)
		{
			@level=l; 
			@level.Add(self);
		}, 
		
		method Reconnect()
		{
			foreach(local l,@outLines)
			{
				l.Points	= Geometry.GetPath(self,l.to);
			}

			foreach(local l,@inLines)
			{
				l.Points	= Geometry.GetPath(l.from,self);
			}
		},
		method GetCx(){
				return (@x+@width/2);
		},//Center x
        method GetCy()	{	return (@GetY()+@height/2);	},//Center y
		method GetX()	{	return @x;					},
        method GetY()	{	return @y;					},
		method GetLevel(){	return @level;				}
			
	];
	Nodes.push_back(tmp);
	return tmp;
}

//-------------------- SCRIPT ---------------------

function NewFile
{
	return [
		@type			: "" ,
		@name			: 0,
        @id				: 0,
        @project		: 0,		
		@dependencies	: list_new(),
        @neighbours		: list_new() ,
		@CurrDependencies	: list_new(),
		@CurrNeighbours	: list_new(),
		@script_deps	: list_new(),
		@unique			: true ,
		@node			: 0 ,
		method Script(script,project)
		{
			@type		= "script";
			@id			= script;
			@project	= project;
			@name		= script.GetName();
				
		},
		method Project(project)
		{
			@type	= "project";
			@id		= project;
			@name	= project.GetName();
		},
		method GetFName()//FullName
		{
			local project	= "";
			if(not @unique)
				project	 	+= @project.name+"/";
			return project+@name;
		},
        method Init()
        {
			@visited	= false;
			@marked		= false;
			@temp_level	= -1;
			@ownList	= list_new();
        },
		method GetNode(Nodes)
		{
			if(not @node)
			{
				@node	= NewNode(self,Nodes);
				@node.name	= @GetFName();
			}
			local end	= listiter_new().setend(Nodes);
			
			if(find(@node,Nodes).equal(end))
				Nodes.push_back(@node);
			return @node;
		}
	];
}

//-------------------------------------------------

//-------------------------------------------------

//-------------------- LEVEL ----------------------

function NewLevel(level_num)
{
	local factorX	= spw::getproperty(::classId , "FactorX").value /100;
	local factorY	= spw::getproperty(::classId , "FactorY").value /100;
	
	function Fx(o, horizonOffset)
	{
		return factorX	*(o.width + horizonOffset);
	}
	function Fy(o, vertOffset)
	{
		return factorY	*(o.height + vertOffset);
	}
    return [
			{"Scripts":list_new()},
            {"num":level_num},
			{"size":0},
			{"largestScriptName"	: ""},
			@topLeftCorner	: 0,
			@width	: 0,
			@height	: 0,
			method SetScripts(spaceBetNodes)
			{
				
				local vert	= spw::getproperty(::classId,"yOffset").value;
					
					local x	= @topLeftCorner.getx() + Renderer.GetTextHeight("A") + spaceBetNodes * factorX ;
					local y	= @topLeftCorner.gety() + Renderer.GetTextHeight("A") ;
					
					local maxX	= 0;
					local maxY	= 0;
					foreach(o,@Scripts)
					{
						
						o.width		= Renderer.GetTextWidth(o.name);
						o.height	= Renderer.GetTextHeight(o.name) ;
						o.GetPath	= Geometry.GetPath;
						o.x			= x;
						o.y			= y;
						if(maxX < o.x + o.width)
							maxX	= o.x + o.width;
							
						if(maxY < o.y + o.height)
							maxY	= o.y + o.height;
						x			= x + Fx(o,spaceBetNodes);
						y			= y + Fy(o,vert);
						
					}
					if(@Scripts.total()!=0)
					{
						@width		= abs( maxX - @topLeftCorner.getx() ) + Renderer.GetTextHeight("A");
						@height		= abs( maxY - @topLeftCorner.gety() ) + Renderer.GetTextHeight("A");
					}
				
			},
			method SetMaxLevel(spaceBetNodes)
			{
				local vert	= spw::getproperty(::classId,"yOffset").value;
				local x	= @topLeftCorner.getx() + Renderer.GetTextHeight("A") + spaceBetNodes * factorX;
				local y	= @topLeftCorner.gety() + Renderer.GetTextHeight("A") + spaceBetNodes * factorY;
				local maxX	= 0;
				local maxY	= 0;
				foreach(o,@Scripts)
				{
					o.width		= Renderer.GetTextWidth(o.name);
					o.height	= Renderer.GetTextHeight(o.name);
					
					o.GetPath	= Geometry.GetPath;
					o.x			= x;
					o.y			= y;
					if(maxX	< o.x + o.width)
						maxX	= o.x + o.width;
					if(maxY	< o.y + o.height)
						maxY	= o.y + o.height;
					
					x			= x + Fx(o,spaceBetNodes);
					y			= y + Fy(o,vert);
				}
				if(@Scripts.total()!=0)
				{
					@width		= abs( maxX - @topLeftCorner.getx() ) + Renderer.GetTextHeight("A");
					@height		= abs( maxY - @topLeftCorner.gety() ) + Renderer.GetTextHeight("A");
				}
			},
			
			method CalculateSize(horizon,vert)
			{
				
				
				local totalWidth	= 0;
				totalWidth	+= Renderer.GetTextHeight("A");
				
				local totalHeight	= 0;
				totalHeight	+= Renderer.GetTextHeight("A");
				
				foreach(o,@Scripts)
				{
					o.width		= Renderer.GetTextWidth(o.name);
					o.height	= Renderer.GetTextHeight(o.name);
					
					totalWidth	+= Fx(o,horizon);
					totalHeight	+= Fy(o,vert);
				}
				totalWidth	+= Renderer.GetTextHeight("A");
				totalHeight	+= Renderer.GetTextHeight("A");

				return 
				[
					@width	: totalWidth,
					@height	: totalHeight
				];
				
			},
			method Draw(dc)
			{
				dc.drawrectangle(@topLeftCorner.getx(),@topLeftCorner.gety(),@width,@height);
			},
			method Add(s)
			{
				@size	+= s.width;
				if(s.width	> strlen(@largestScriptName))
					@largestScriptName = s.name;
				@Scripts.push_back(s);
			},
            method AddLevelFrom(l,levels)
			{
				local next=find(self,levels).fwd();
                for(i=@num+1;i<=@num+l;i++)
					levels.insert(next,NewLevel(i));
                    for(;not next.equal(listiter_new().setend(levels));next.fwd())
                    {
						next.getval().num+=l;
                    }
                    return find(self,levels).fwd().getval();
            },
            method GetScripts(){return @Scripts;},
            method GetY()
			{
				local VertOffset	=	spw::getproperty(::classId, "LevelsDistance").value;
				return @num*VertOffset+VertOffset/2;
			},
           
            
			method Put(node)
			{
				node.level	= self;
				@Scripts.push_back(node);

			}
    ];
}

//-------------------------------------------------

//------------------- DATA --------------------

function NewLine(Points,from,to,Lines)
{
	local tmp= [
				{"Points"	: Points		},
				{"from"		: from			},
				{"to"		: to			},
				{"colour"	: "LineColour"	},
				{"width"	: "DefaultLineWidth"	},
				method Reset()
				{
					@width	= "DefaultLineWidth";
					@colour	= "LineColour";
				}
	];
	Lines.push_back(tmp);
	return tmp;
}

function CheckIfUniqueName(s,List)
{
	foreach(obj,List)
	{
		if((obj!= s) and (obj.name == s.name))
		{
			obj.unique = false;
			s.unique = false;
			return ;
		}
	}
}

function CollectData
{
	function AddScript
	{
		return 
		[
			@Scripts	: [],
			@Projects	: [],
			@Lines		: list_new(),
			@Nodes		: list_new(),
			@Other		: list_new(),
			method WatchDependencies(file,dependencies)
			{
				foreach(local child ,dependencies)
				{
					child=spw.decorate(child);
					if(@Scripts[child.serial]==nil)
					{
						self(child,file);
					}
					else
					{
						end_list=listiter_new().setend(@Scripts[child.serial].neighbours);
						if(listiter_equal(find(@Scripts[file.id.serial],@Scripts[child.serial].neighbours),end_list)==true)
							@Scripts[child.serial].neighbours.push_back(@Scripts[file.id.serial]);
					}
					file.dependencies.push_back(@Scripts[child.serial]);
				
					if(@Scripts[child.serial].project!=@Scripts[file.id.serial].project)
					{
						local end_list=listiter_new().setend(@Projects[@Scripts[file.id.serial].project.id.serial].dependencies);
						if(listiter_equal(find(@Projects[@Scripts[child.serial].project.id.serial],@Projects[@Scripts[file.id.serial].project.id.serial].dependencies),end_list)==true)
						{
							@Projects[@Scripts[file.id.serial].project.id.serial].dependencies.push_back(@Projects[@Scripts[child.serial].project.id.serial]);
							@Projects[@Scripts[child.serial].project.id.serial].neighbours.push_back(@Projects[@Scripts[file.id.serial].project.id.serial]);
						}
					}
				}
				return 0;
			},
			{"()":	
				method (script,from)
				{
					script 		= spw.decorate(script);
					
					if(@Scripts[script.serial])
					{
						if(from)
						{
							end_list=listiter_new().setend(@Scripts[script.serial].neighbours);
							if(listiter_equal(find(@Scripts[from.id.serial],@Scripts[script.serial].neighbours),end_list)==true)
								@Scripts[script.serial].neighbours.push_back(@Scripts[from.id.serial]);
						}
						return;
					}
					local dependencies = script.GetDependencies()[1];
					
					local temp 	= script;
					while (temp.GetParent().class_id!="Project")
					{
						temp = temp.GetParent();
					}

					local project = temp.GetParent();
					
					if(@Projects[project.serial]==nil)
					{
						local file	= NewFile();
						file.Project(project);
						@Projects[project.serial]	= file;
						
					}
	   
					if(@Scripts[script.serial]==nil)
					{
						local file	= NewFile();
						file.Script(script,@Projects[project.serial]);
						@Scripts[script.serial]	= file;
												
						CheckIfUniqueName(@Scripts[script.serial],@Scripts);
					}
					
					end_it=listiter_new().setend(@Projects[project.serial].script_deps);
					if(listiter_equal(find(@Scripts[script.serial],@Projects[project.serial].script_deps),end_it)==true)
						@Projects[project.serial].script_deps.push_back(@Scripts[script.serial]);
					if(from!=nil)
					{
						end_list=listiter_new().setend(@Scripts[script.serial].neighbours);
						if(listiter_equal(find(@Scripts[from.id.serial],@Scripts[script.serial].neighbours),end_list)==true)
							@Scripts[script.serial].neighbours.push_back(@Scripts[from.id.serial]);

						if(@Scripts[from.id.serial].project!=@Scripts[script.serial].project)
						{
							@Scripts[script.serial].project.neighbours.push_back(@Scripts[from.id.serial].project);//fix
							@Scripts[from.id.serial].project.dependencies.push_back(@Scripts[script.serial].project);

						}
					}
					@WatchDependencies(@Scripts[script.serial],dependencies);
	   
				}
			}
		];
		
	}
	function For_each(t,f){	foreach(x,t) f(x,nil); return f;}
	local scripts = spw.components.ProjectManager.GetResources("Script");
	return For_each(scripts,AddScript());
}
//------------- END OF DATA ----------------------

//------------- GEOMETRY -------------------------

//				GEOMETRY2

function Distance(A,B)
{
    return sqrt(sqr(B.gety()-A.gety())+sqr(B.getx()-A.getx()));
}

function GetCloser(Min,A,B)
{
	
    if (Min==nil or (Distance(A,B)<Distance(A,Min)))
        return point_construct(B);
    else
        return point_construct(Min);
}

function Between(C,A,B)
{
    return (C>=A and C<=B);
}

function CollisionPoint(A,B)
{
    local p=point_construct(0,0);
    local dy = B.GetCy()-A.gety();
    local dx = B.GetCx()-A.getx();
    local l=0;
   
    if(dx!=0)
    {
        if(A.gety()==B.GetCy())
        {
            if(A.getx()>B.GetCx())
            {
                p.sety(A.gety());
                p.setx(B.GetX()+B.width);
            }
            else
            {
                p.sety(A.gety());
                p.setx(B.GetX());
            }
            return p;
        }
        l=dy/dx;
        local pA=point_construct(A.getx(),A.gety());
       
        local Min=nil;
        local tempP;
        if(Between(l*(B.GetX()-B.GetCx())+B.GetCy(),B.GetY(),B.GetY()+B.height)) //left
        {
            tempP=point_construct(B.GetX(),l*(B.GetX()-B.GetCx())+B.GetCy());
            Min=GetCloser(Min,pA,tempP);
        }
        if(Between(l*(B.GetX()+B.width-B.GetCx())+B.GetCy(),B.GetY(),B.GetY()+B.height)) //right
        {
            tempP=point_construct(B.GetX()+B.width,l*(B.GetX()+B.width-B.GetCx())+B.GetCy());
            Min=GetCloser(Min,pA,tempP);
        }
        if(Between((B.GetY()-B.GetCy())/l+B.GetCx(),B.GetX(),B.GetX()+B.width))//up
        {
            tempP=point_construct((B.GetY()-B.GetCy())/l+B.GetCx(),B.GetY());
            Min=GetCloser(Min,pA,tempP);
        }
        if(Between((B.GetY()+B.height-B.GetCy())/l+B.GetCx(),B.GetX(),B.GetX()+B.width)) //down
        {
            tempP=point_construct((B.GetY()+B.height-B.GetCy())/l+B.GetCx(),B.GetY()+B.height);
            Min=GetCloser(Min,pA,tempP);
        }
        return Min;
		
    }
    else
    {
		local NodeHeight	=	B.height;
        if(A.gety()>B.GetCy()){ p.setx(A.getx()); p.sety(B.GetY()+NodeHeight);}
        else { p.setx(A.getx()); p.sety(B.GetY()); }
    }
    return p;
}

function MaxLevel(Levels)
{
	if(Levels.total()!=0)
	{
		local factorX	= spw::getproperty(::classId,"FactorX").value;
		local maxLevel = Levels.front();

		if(factorX !=0)
		{			
			foreach(l,Levels)
			{
				if(l.size>maxLevel.size)
					maxLevel=l;
			}
		}
		else{
			foreach(l,Levels)
			{
				if(l.Scripts.total()>maxLevel.Scripts.total())
					maxLevel=l;
			}
		}
		return maxLevel;
	}
}

function G1(Data)
{	
	function Array2List(array)
	{
		local list=list_new();
		foreach (a,array)
		{
			list.push_back(a);
		}
	   
		return list;
	}
	
	function Sort(List)
	{
		local i=listiter_new();
		for (i=i.setbegin(List);(not (i.checkend(List)));i=i.fwd())
		{
		   
			local script1=i.getval();
			local j=listiter_new();
			for (j.assign(i);(not (j.checkend(List)));j=j.fwd())
			{
				local script2=j.getval();
			   
				if(script1.neighbours.total()>script2.neighbours.total())
				{
					temp=i.getval();
					i.setval(j.getval());
					j.setval(temp);
					script1=i.getval();
				}
			}
		}
		local j_temp=listiter_new();
		for (i = i.setbegin(List);(not (i.checkend(List)));i=i.fwd())
		{
			local script1=i.getval();
			local j=listiter_new();
			j_temp.assign(i);
			for (j.assign(j_temp);( (not (j.checkend(List))) and (j.getval().dependencies.total()==i.getval().dependencies.total()));j=j.fwd())
			{
				local script2=j.getval();
				if(not find(script1,script2.dependencies).equal(listiter_new().setend(script2.dependencies)) or script1.dependencies.total()<script2.dependencies.total())
				{
					
					temp=i.getval();
					i.setval(j.getval());
					j.setval(temp);
					script1=i.getval();
				}           
			}
		}
	   
		return List;
	}
	local scripts	=
	Data.Scripts	= Sort(Array2List(Data.Scripts));
	local projects	=
	Data.Projects	= Sort(Array2List(Data.Projects));
	local levels	= list_new();
	local view		= "";
    local currList	= list_new();
	local singles	= list_new();
    local commons	= list_new();
    local roots		= list_new();
	local lines		= Data.Lines;
	local nodes		= Data.Nodes;
	local other		= Data.Other;
	local spaceBetNodes	= 0;
	
	
	function Neighbours(s)
	{
		list = list_new();
		foreach(o,s.neighbours)
			if(not find(o,currList).equal(listiter_new().setend(currList)))
				list.push_back(o);
		return list;
	}
       
	function Dependencies(s)
	{
		list = list_new();
		foreach(o,s.dependencies)
			if(not find(o,currList).equal(listiter_new().setend(currList)))
				list.push_back(o);
		return list;
	}
	
	function Init(obj)
	{
		obj.temp_level	= -1;
		obj.marked		= false;
		obj.visited		= false;
		obj.ownList		= list_new();
		
		obj.CurrDependencies	= Dependencies(obj);
		obj.CurrNeighbours		= Neighbours(obj);
		
		local node	= obj.GetNode(nodes);
		node.name	= obj.GetFName()+"{users:"+obj.CurrDependencies.total()+" , used:"+obj.CurrNeighbours.total()+" } ";
		
		node.Reset();
	}
	
	function ListCopy(List)
	{
		new_list = list_new();
		foreach(l,List)
			new_list.push_back(l);
		return new_list;
	}
	
	function UpdateOwnlist(s)
	{
		foreach(o,s.ownList)
		{
			o.temp_level=s.temp_level+1;
			UpdateOwnlist(o);
		}
	}
	
	function FindObstacles(p1,B,temp_array,lev,Levels)
	{
		local NodeHeight	= 20;
		local p2	= point_construct(B.GetCx(),B.GetCy());
		tabset(temp_array , tablength(temp_array) , p1);
		if(lev==B.level)
		{
			tabset(temp_array , tablength(temp_array) ,point_construct(B.GetCx(),B.GetCy()) );
			return temp_array;
		}
		local x=0,y=0,x1=0,y1=0,x2=0,y2=0;
		if(p1.gety()==p2.gety())
		{
			x=p2.getx();
			y=p2.gety();
		}
		else
		{
			if(p1.getx()==p2.getx())
			{
				x=p1.getx();
				y=lev.getY();
				y1=lev.getY();
				x1=p1.getx();
				y2=lev.getY()+NodeHeight;
				x2=p2.getx();
			}    
			else
			{
				local l =(p2.gety()-p1.gety())/(p2.getx()-p1.getx());
				y1=lev.GetY();
				x1=(y1-B.GetCy())/l+B.GetCx();
				y2=lev.GetY()+NodeHeight;
				x2=(y2-B.GetCy())/l+B.GetCx();
				y=lev.GetY()+NodeHeight/2;
				x=(y-B.GetCy())/l+B.GetCx();
			}
			foreach(s,lev.Scripts)
			{
				if( (s.GetX()<=x1 and x1<=s.GetX()+s.width) or (s.GetX()<=x2 and x2<=s.GetX()+s.width))
				{
					if(Distance(p1,point_construct(x1,y1))<Distance(p1,point_construct(x2,y2)))
					  x=x1;
					else
					  x=x2;
					
					if(x>s.GetX()+s.width/2 )
					{
					  x+=s.width/2;
					}
					else
					{
					  x-=s.width/2;
					}
					break;
						   
				}
			}
		}
		if(p1.gety()<B.GetCy())
		{
			temp_array	= FindObstacles(point_construct(x,y),B,temp_array,find(lev,levels).fwd().getval(),levels);
		}
		else
		{
			temp_array	= FindObstacles(point_construct(x,y),B,temp_array,find(lev,Levels).bwd().getval(),levels);
		}
		return temp_array;
	}
	
	function PutRoots
	{
		foreach(s,roots)
		{
			local node	= s.node;
			node.SetLevel(levels.front());
			node.visited	= true;
			node.marked		= true;
			foreach(o,s.ownList)
			{
				o.temp_level = s.temp_level+1;
				UpdateOwnlist(o);
			}
		}
	}

	function CheckCommons
	{
		if(commons.total()==0)
			return false;
		foreach(c,commons)
		{
			counter=0;
			local neighs 	= c.CurrNeighbours;
			local Max		= neighs.front();
			foreach(s,neighs)
			{
				if(not s.marked)
					break;
				else if(s.temp_level>Max.temp_level)
					Max=s;
				counter++;
			}
			if(counter==neighs.total())
			{
				Max.ownList.push_back(c);
				c.marked=true;
				c.temp_level=Max.temp_level+1;
				UpdateOwnlist(c);
				commons.remove(c);
				return true;
			}
                
		}
		return false ;
	}
	    /*
        AdditionalDeps of script s returns all children-scripts that are not visited
        */
    function AdditionalDeps(s)
	{
		foreach(local o,s.ownList)
        {
			if(o.visited)
				continue;
				
            o.visited	= true;
				
            local l;
			local sNode	= s.node;
			local oNode	= o.node;
            if(sNode.GetLevel().num==levels.total()-1)
				l=sNode.GetLevel().AddLevelFrom(1,levels);
			else
				l=find(sNode.GetLevel(),levels).fwd().getval();

			oNode.SetLevel(l);
			AdditionalDeps(o);
		}
	}
	
    function Map()
	{
        if(roots.total()==0)
        {
			local node	= currList.front().node;
			node.SetLevel(levels.front());
			node.visited	= true;
		}

        foreach(obj,currList)
        {
            //obj.dependencies=array2list(obj.dependencies);
			/*if(not find(obj,Roots).equal(listiter_new().setend(Roots)))
            {
				obj.visit();
				obj.setLevel(Levels.front());
				Roots.remove(obj);
			}*/
            AdditionalDeps(obj);   
		}
	}
	
return [
	@Nodes 	: nodes,
	@Lines 	: lines,
	@Other	: other,
	
	method ConnectScripts(A,B)
	{
		local points	= @GetPath(A,B);
		local line		= NewLine(points,A,B,lines);

		A.outLines.push_back(line);
		B.inLines.push_back(line);		
	},
	method GetPath(A,B)
	{
		p1		= point_construct(A.GetCx(),A.GetCy());
		local points,p2;
		p2=point_construct(B.GetCx(),B.GetCy());

		p2 			= CollisionPoint(p1,B);
		points 		= [p1,point_construct(B.GetCx(),B.GetCy())];
		
		tabset(points , tablength(points)-1,p2);
		return points;
	},
	method SetLevels(maxlevel)
	{
		local spaceBetNodes	= 
		local maxLevelSpace	= spw::getproperty(::classId,"SpaceBetNodes").value;
		local maxLevelsize	= maxlevel.CalculateSize(spaceBetNodes, spaceBetNodes);
		local maxWidth	= maxLevelsize.width;
		local maxHeight	= maxLevelsize.height;
		local tmpLeftCorner	= point_construct(10,10);
		local levelsArrangement	= "VERTICAL";
		local factorX	= spw::getproperty(::classId , "FactorX").value /100;
		local factorY	= spw::getproperty(::classId , "FactorY").value /100;
		if(factorX == 0 and factorY	== 1)
			levelsArrangement	= "HORIZONTAL";
		local startPoint	= point_construct(10,10);
		tmpLeftCorner	= startPoint;
		local levelsDistance	= spw::getproperty(::classId,"LevelsDistance").value;
		foreach(l,levels)
		{
			l.topLeftCorner	= point_construct( tmpLeftCorner.getx() , tmpLeftCorner.gety() );
			if(l==	maxlevel)
			{
				l.SetMaxLevel(maxLevelSpace);
			}
			else
			{
				
				spaceBetNodes	= (maxWidth-l.size)/(l.Scripts.total()+1);
				l.SetScripts(spaceBetNodes);

			}
			if(levelsArrangement	== "VERTICAL")
				tmpLeftCorner.sety( l.topLeftCorner.gety() + l.height + levelsDistance );
			else
				tmpLeftCorner.setx(	l.topLeftCorner.getx() + l.width + levelsDistance);
			
		}
	},

	method reapply()
	{
		@Apply(@lastList);
	},
		
	method Apply(list)
	{
		@lastList	= list;
			
		if(list.type and list.type	== "script")
		{
			currList	= Sort(ListCopy(list.dependencies));
			currList.push_front(list);
		}
		else if(list.type and list.type	== "project")
		{
			currList	= Sort(ListCopy(list.script_deps));
			currList.push_front(list);
		}
		else
			currList=Sort(ListCopy(list));

		
		if(currList.total()==0)
		{
			Renderer.Print("There are no dependencies!");
			return;
		}
		spaceBetNodes	 = spw::getproperty(::classId, "SpaceBetNodes").value;

		nodes.clear();
		lines.clear();
		other.clear();
		
		commons=list_new();
		singles=list_new();
		roots=list_new();
		levels=list_new();
		
		foreach(obj,currList)
			Init(obj);
		foreach(obj,currList)
		{
			local end	= listiter_new().setend(commons);
			local neighs= obj.CurrNeighbours;
			local deps	= obj.CurrDependencies;
			
			
/*Commons*/	if( neighs.total()>1 and (find(obj,commons).equal(end)==true))
			{

				commons.push_back(obj);
			}
/*OwnList*/	else if(neighs.total()==1 and not find(neighs.front(),currList).equal(listiter_new().setend(currList)) )//if(file.dependencies==0 and file.neighbours==0)
			{

				obj.marked=true;
				neighs.front().ownList.push_back(obj);//this file belongs to the unique neighbour
			}
/*Singles*/	else if(neighs.total()==0 and deps.total()==0)
			{

				obj.marked=true;
				singles.push_back(obj);
				currList.remove(obj);
				nodes.remove(obj.node);
				obj.node	= 0;
				obj.temp_level=0;
			}
/*Roots*/	else if(neighs.total()==0 )
			{

				roots.push_back(obj);
				obj.temp_level=0;
				obj.marked=true;
			}
		}
//sos				foreach(obj,@CurrList)
//sos					obj.WidthSize = dc.gettextextent(obj.GetFName()).getwidth();
				
		if(currList.total()==0)
		{
			Renderer.Print("There are no dependencies!\n");
			return;
		}
		
		levels.push_back(NewLevel(0));
		PutRoots();
		
		while(CheckCommons()){}
		Renderer.Print("Mapping", 45);	
		Map();
		Renderer.Print("FitSpace", 50);
		@FitSpace();
		Renderer.Print("Ready", 100);
	},
	
		method DetectObject(x,y)
	{
		local i=listiter_new();
		for (i=i.setend(@Nodes).bwd();(not (i.checkbegin(@Nodes)));i=i.bwd())
		{
			local n	= i.getval();
			if( (x>=n.x and x<=n.x+n.width) and (y>=n.y and y<=n.y+n.height))
			{
				return n;
			}
		}
		return nil;
	},

	method FitSpace
	{
		local l = MaxLevel(levels);
		
		Renderer.Print("SetLevels", 55);
		
		@SetLevels(l);
		Renderer.Print("After SetLevels");
		foreach(s,currList)
		{
			s.node.outLines = list_new();
			s.node.inLines 	= list_new();
		}
		
		foreach (s,currList)
		{
			foreach(o, s.CurrDependencies)
			{
				@ConnectScripts(s.node,o.node);
			}
		}
		Renderer.Print("End of load",100);

	},
	method DisplayProjects()
	{
		@Apply(projects);
	},
	method ApplyScripts()
	{
		@Apply(scripts);
	}

	];
}

function G2(Data)
{
	local scripts	= Data.Scripts;
	local projects	= Data.Projects;
	local nodes		= Data.Nodes;
	local lines		= Data.Lines;
	local other		= Data.Other;
	local levels	= list_new();
	function FindObstacles(p1,B,temp_array,lev,Levels)
	{
		local NodeHeight	= Renderer.GetTextHeight("A");
		local p2	= point_construct(B.getCx(),B.getCy());
		tabset(temp_array , tablength(temp_array) , p1);
		if(lev==B.level)
		{
			tabset(temp_array , tablength(temp_array) ,point_construct(B.getCx(),B.getCy()) );
			return temp_array;
		}
		local x=0,y=0,x1=0,y1=0,x2=0,y2=0;
		if(p1.gety()==p2.gety())
		{
			x=p2.getx();
			y=p2.gety();
		}
		else
		{
			if(p1.getx()==p2.getx())
			{
				x=p1.getx();
				y=lev.getY();
				y1=lev.getY();
				x1=p1.getx();
				y2=lev.getY()+NodeHeight;
				x2=p2.getx();
			}    
			else
			{
				local l =(p2.gety()-p1.gety())/(p2.getx()-p1.getx());
				y1=lev.getY();
				x1=(y1-B.getCy())/l+B.getCx();
				y2=lev.getY()+NodeHeight;
				x2=(y2-B.getCy())/l+B.getCx();
				y=lev.getY()+NodeHeight/2;
				x=(y-B.getCy())/l+B.getCx();
			}
			foreach(s,lev.Scripts)
			{
				if( (s.getX()<=x1 and x1<=s.getX()+s.width) or (s.getX()<=x2 and x2<=s.getX()+s.width))
				{
					if(Distance(p1,point_construct(x1,y1))<Distance(p1,point_construct(x2,y2)))
					  x=x1;
					else
					  x=x2;
					
					if(x>s.getX()+s.width/2 )
					{
					  x+=s.width/2;
					}
					else
					{
					  x-=s.width/2;
					}
					break;
						   
				}
			}
		}
		if(p1.gety()<B.getCy())
		{
			temp_array	= FindObstacles(point_construct(x,y),B,temp_array,find(lev,levels).fwd().getval(),levels);
		}
		else
		{
			temp_array	= FindObstacles(point_construct(x,y),B,temp_array,find(lev,levels).bwd().getval(),levels);
		}
		return temp_array;
		
	}
	
return [
	@Nodes 	: nodes,
	@Lines 	: lines,
	@Other	: other,
	method ConnectScripts(A,B)
	{
		local points	= @GetPath(A,B);
		local line		= NewLine(points,A,B,lines);

		A.outLines.push_back(line);
		B.inLines.push_back(line);		
	},
	method GetPath(A,B)
	{
		local points=[],p2;
		
		p1	= point_construct(A.GetCx(),A.GetCy());
		p2	= point_construct(B.GetCx(),B.GetCy());

		p2 		= CollisionPoint(p1,B);
		points	= [p1,point_construct(B.GetCx(),B.GetCy())];

		tabset(points , tablength(points)-1,p2);
		return points;
	},
	method SetLevels(maxLevel)
	{
		local spaceBetNodes	= 
		local maxLevelSpace	= spw::getproperty(::classId,"SpaceBetNodes").value;
		local maxLevelsize	= maxLevel.CalculateSize(spaceBetNodes, spaceBetNodes);
		local maxWidth	= maxLevelsize.width;
		local maxHeight	= maxLevelsize.height;
		local tmpLeftCorner	= point_construct(10,10);
		local levelsArrangement	= "VERTICAL";
		local factorX	= spw::getproperty(::classId , "FactorX").value /100;
		local factorY	= spw::getproperty(::classId , "FactorY").value /100;
		if(factorX == 0 and factorY	== 1)
			levelsArrangement	= "HORIZONTAL";
		local startPoint	= point_construct(10,10);
		tmpLeftCorner	= startPoint;
		local levelsDistance	= spw::getproperty(::classId,"LevelsDistance").value;
		foreach(l,levels)
		{
			l.topLeftCorner	= point_construct( tmpLeftCorner.getx() , tmpLeftCorner.gety() );
			if(l==	maxLevel)
			{
				l.SetMaxLevel(maxLevelSpace);
			}
			else
			{
				
				spaceBetNodes	= (maxWidth-l.size)/(l.Scripts.total()+1);
				l.SetScripts(spaceBetNodes);

			}
			if(levelsArrangement	== "VERTICAL")
				tmpLeftCorner.sety( l.topLeftCorner.gety() + l.height + levelsDistance );
			else
				tmpLeftCorner.setx(	l.topLeftCorner.getx() + l.width + levelsDistance);
			//tmpLeftCorner	= point_construct(tmpLeftCorner);
		}
			
		
	},
	method DetectObject(x,y)
	{
		local i=listiter_new();
		for (i=i.setend(@Nodes).bwd();(not (i.checkbegin(@Nodes)));i=i.bwd())
		{
			local n	= i.getval();
			if( (x>=n.x and x<=n.x+n.width) and (y>=n.y and y<=n.y+n.height))
			{
				return n;
			}
		}
		return nil;
	},
	method reapply()
	{
		@Apply(@lastList);
	},
	method Apply(currList)
	{
		@lastList	= currList;
		function NewField(name,from,	to, Levels)
		{
			return [
				@name	: name,
				@from	: from,
				@to		: to,
				method Draw(dc)
				{
					local iter	= find(from,Levels);
					local x	= @from.topLeftCorner.getx();
					local y	= @from.topLeftCorner.gety();
					local width	= MaxLevel(Levels).width;
					local height= abs(@from.topLeftCorner.gety() - (@to.topLeftCorner.gety() + @to.height ) );
					
					Renderer.BrushConstruct(dc,"BGColour");
					Renderer.RoundRect(dc,x,y,width,height);
					
					Renderer.DrawText(dc," "+@name,x,y);
					
				}
			];
		}
		
		nodes.clear();
		lines.clear();
		other.clear();
		
		
		levels=list_new();
		
		local scriptsPerLevel		= spw::getproperty(::classId, "ScriptsPerLevel").value;
		levels.push_back(NewLevel(0));
		Renderer.Print("=== Loading Graph ===", 60);
		spaceBetNodes	=	spw::getproperty(::classId, "SpaceBetNodes").value;
		//local CurrList	= Scripts;
		foreach(s,currList)
		{
			s.CurrDependencies	= s.dependencies;
			s.CurrNeighbours	= s.neighbours;
		}
		local level	= 0;
		local fields	= list_new();
		other.push_back(fields);
		foreach(p,projects)
		{
			counter		= 1;
			tempLevel	= levels.back();
			
			local fromLevel	= tempLevel;
			foreach(s,p.script_deps)
			{
				local node	= 
				s.node		= NewNode(s,nodes);
				node.GetPath= self.GetPath;
				node.script	= s;
				local name	= s.GetFName();
				local width	= Renderer.GetTextWidth(name);
				local height= Renderer.GetTextHeight(name);
				node.name	= name;
				node.width	= width;
				node.height	= height;
				node.SetLevel(tempLevel);
				if(counter % scriptsPerLevel == 0)
				{
					levels.push_back(NewLevel(levels.total()));
					tempLevel	= levels.back();
				}
				counter++;
			}
			
			local toLevel	= tempLevel;
			
			if(tempLevel.Scripts.total()!=0)
				levels.push_back(NewLevel(levels.total()));
			else
				toLevel	= find(tempLevel,levels).bwd().getval();
			local currField	= NewField(p.name,fromLevel, toLevel, levels);
			fields.push_back(currField);
			
		}
		local l = MaxLevel(levels);
		
		levelWidth	= l.size + ((l.Scripts.total()-1)*spaceBetNodes);
		
		@SetLevels(l);
		Renderer.Print("=== Connect ===", 80);
		
		foreach(s,scripts)
		{
			foreach(d,s.CurrDependencies)
			{
				@ConnectScripts(s.node ,d.node);
			}
		}
		Renderer.Print("=== Ready ===", 100);
		
	},
	method DisplayProjects()
	{
		
		@Apply(projects);
	},
	method ApplyScripts()
	{
		@Apply(scripts);
	}
	
	];
}

//------------------------------------------------

//~~~~~~~ INTERACT ~~~~~~~~~
function Zoomout(window)
{
	window.userscale		*= 1.2;
	window.refresh();
	//Renderer.Repaint(window);
}
function Zoomin(window)
{
	window.userscale		/= 1.2;
	window.refresh();
	//Renderer.Repaint(window);
}
function Interact1
{
	local selected	= nil;
	local lastX	= 0;
	local lastY	= 0;
	return [
		method DeviceToLogical (window, x, y)
		{
			local pt		= window.calcunscrolledposition(x, y);
			pt.setx(pt.getx() / window.userscale);
			pt.sety(pt.gety() / window.userscale);
			return pt;
		},
		method OnCharPressed(window, ev)
		{
			local plusSymbol	= "+";
			local plusASCII	= strbyte(plusSymbol,0);
	
			local minusSymbol	= "-";
			local minusASCII	= strbyte(minusSymbol,0);
			
			local pSymbol	= "p";
			local pASCII	= strbyte(pSymbol,0);
			
			local sSymbol	= "s";
			local sASCII	= strbyte(sSymbol,0);
			
			local unicode	= ev.getunicodekey();
			local keycode	= ev.getkeycode();
						
			if( ( unicode	== "P" or unicode	== "p"  ) )
				{	
					Renderer.ViewProjects();
				}
			else if( ( unicode	== "S" or unicode	== "s" ) )
					Renderer.ViewScripts();
			else if(ev.controldown()) {
				
				if( ( keycode	== plusASCII ) /*and (ev.controldown()) */)
				{	Zoomin(window);
				}else if( ( keycode	== minusASCII ) /*and (ev.controldown())*/ )
				{	Zoomout(window);
				}else
				{	ev.skip();
				}
			}			
		},
		method OnLeftDClickEvent(window,evt)
		{
			local dc	= window.Maindc;
			
			local logicalPos= @DeviceToLogical(window, evt.getx(), evt.gety());
			local logicalX	= logicalPos.getx();
			local logicalY	= logicalPos.gety();
//			local logicalX	= dc.devicetologicalx(evt.getx());
//			local logicalY	= dc.devicetologicaly(evt.gety());
			
			local obj = currGeometry.DetectObject(logicalX,logicalY);
			selected = obj;
			
			if(obj)
			{
				currGeometry	= fileGeometry;
				fileGeometry.Apply(obj.file);
				selected	= nil;
				Renderer.PrepareGraph();
				window.refresh();
//				Renderer.Repaint(window);
			}
			
		},

		method OnLeftDownEvent(window,evt)
		{
			local dc	= window.Maindc;

			local logicalPos= @DeviceToLogical(window, evt.getx(), evt.gety());
			local logicalX	= logicalPos.getx();
			local logicalY	= logicalPos.gety();
			
			local obj = currGeometry.DetectObject(logicalX, logicalY);
			
			if(obj!=selected and selected)
				selected.Reset();
				
			selected = obj;
			if(selected)
			{
				selected.Highlight();
				local nodes	= Data.Nodes;
				//spw::print(nodes.total(),nl);
				//spw::print(nodes.remove(selected),nl);
				//spw::print(nodes.total(),nl);

				nodes.push_back(selected);
			}
			
			lastX	= logicalX;
			lastY	= logicalY;
			
			window.refresh();
		},
		method OnLeftUpEvent(window,evt)
		{
			local dc	= window.Maindc;
			local logicalPos= @DeviceToLogical(window, evt.getx(), evt.gety());
			local logicalX	= logicalPos.getx();
			local logicalY	= logicalPos.gety();
			

			if(selected)
			{
				selected.StopMoving();
			}
			window.refresh();
//			Renderer.Repaint(window);
			
		},

		method OnMotionEvent(window, evt) {
			if (evt.leftisdown()) {
				local dc	= window.Maindc;
				local logicalPos= @DeviceToLogical(window, evt.getx(), evt.gety());
				local logicalX	= logicalPos.getx();
				local logicalY	= logicalPos.gety();
				
				if(selected)
				{
					selected.Move(logicalX - lastX, logicalY - lastY);
					lastX	= logicalX;
					lastY	= logicalY;
					
					selected.Reconnect();
					window.refresh();
				}

			}
		},
		method Init(window)
		{
			window.connect("EVT_LEFT_DOWN",	 @OnLeftDownEvent	);
			window.connect("EVT_LEFT_UP",	 @OnLeftUpEvent	);
			window.connect("EVT_LEFT_DCLICK",@OnLeftDClickEvent	);
			window.connect("EVT_MOTION", 	 @OnMotionEvent	);
			window.connect("EVT_KEY_DOWN",	 @OnCharPressed	);
		}
	];
}
//------------------------------------------------

//~~~~~~~ RENDERER ~~~~~~~~~
function wxRenderer {
	

	return [
		@Lines	: 	0,
		@Nodes	: 	0,
		@Other	:	0,
		@message: 	0,

		method BrushConstruct(dc,c)
		{
			local colour	= @GetColour(c);
			dc.setbrush(brush_construct(colour));
		},
		method DrawText(dc,text,x,y)
		{
			dc.drawtext(text,x,y);
		},
		method GetColour(id)
		{
			local obj = spw::getproperty(::classId, id).value;
			return colour_construct(obj.r, obj.g, obj.b);
		},
		method GetFont(id)
		{
			local obj	= spw::getproperty(::classId, id).value;
			local size	= obj.size;
			local family= obj.family;
			local style	= obj.style;
			local weight= obj.weight;

			return font_construct( size, family, style, weight, false ,"Arial");
		},
		
		method Rect(dc,x,y,width,height)
		{
			wx::dc_drawrectangle(dc,x,y,width,height);
		},
		method RoundRect(dc,x,y,width,height,BGColour)
		{
			
			local arc	=spw::getproperty(::classId,"ShapeArc").value;
			wx::dc_drawroundedrectangle(dc,x,y,width,height,arc);
		},
		method GetTextWidth	(text)
		{
			local dc	= buffereddc_construct(window.Maindc);
			dc.setfont(@GetFont("Font"));
			return dc.gettextextent(text).getwidth();
		},
		method GetTextHeight(text)
		{
			local dc	= buffereddc_construct(window.Maindc);
			dc.setfont(@GetFont("Font"));
			return dc.gettextextent(text).getheight();
		},
		method DisplayOther(dc)
		{
			foreach(list,@Other)
			{
				foreach(o ,list)
				{
					o.Draw(dc);
				}
			}
			
		},
		method DisplayNodes(dc)
		{
			local maxX	= 0;
			local maxY	= 0;
			counter =1;
			local shape_index	= spw::getproperty(::classId,"NodeShape").index;
			local shapes	= [@RoundRect,@Rect];
			local shape	= shapes[shape_index];
			
			foreach(node,@Nodes)
			{
				local x			= node.x;
				local y			= node.y;
				local width		= node.width;
				local height	= node.height;	
				local name		= node.name;
				local BorderWidth		= spw::getproperty(::classId,node.BorderWidth).value;
				if(	x + width > maxX )
					maxX	= x + width;
				if( y + height > maxY )
					maxY	= y + height;
					
				local brush_colour	= @GetColour(node.BGcolour);
				local brush			= brush_construct(brush_colour);
				
				local pen_colour	= @GetColour(node.Shape_Colour);
				local pen			= pen_construct(pen_colour,BorderWidth);
				
				dc.setbrush(brush);
				dc.setpen(pen);
				shape(dc,x,y,width,height);
				dc.drawtext(name,x,y);
			}
			
			window.setvirtualsize(maxX, maxY);
		},
		method DrawPointer(dc,p1,p2)
		{
			dc.setbrush( brush_construct(@GetColour("PointerColour")));
			
			
			local Ax	= p1.getx();
			local Ay	= p1.gety();
			
			local Bx	= p2.getx();
			local By	= p2.gety();
			
			local length= spw::getproperty(::classId,"PointerLength").value;
			local f		= spw::getproperty(::classId,"PointerAngle").value;
			
			local D1y 	= nil;
			local D1x 	= nil;
	 
			local D2y 	= nil;
			local D2x 	= nil;
			
			local Cy 	= nil;
			local Cx 	= nil;
			
			if(Bx	!= Ax)
			{	
	 
				local BC	= length;
				local AB	= sqrt(sqr(By-Ay) + sqr(Bx-Ax));

				
				local AC	= AB-BC;//%BC*sin(f) %
				local d		=( sqr(BC) - sqr(AC) - sqr(Bx) + sqr(Ax) - sqr(By) + sqr(Ay) ) / (2*Ax - 2*Bx);
				
				local e		= (By-Ay)/(Ax-Bx);
				
				local l 	= (By-Ay)/(Bx-Ax);
				
				Cy 	= (By + l*d - l*Bx) / (1- l*e);
				Cx 	= d + e*Cy;
 
				BD	= length/ cos(f);
				CD	= BD*sin(f); 

				d	=( sqr(BD) - sqr(CD) - sqr(Bx) + sqr(Cx) - sqr(By) + sqr(Cy) ) / (2*Cx - 2*Bx);
	 
				e	= (By-Cy)/(Cx-Bx);
	 
				a  	= 1+ sqr(e);
				b  	= 2*d*e - 2*e*Cx - 2*Cy;
				c  	= sqr(d) - 2*d*Cx + sqr(Cy) + sqr(Cx) - sqr(CD);
	 
				local Delta = sqr(b) - 4*a*c;
				D1y 	= (-b + sqrt(Delta))/(2*a);
				D1x 	= d + e*D1y;
	 
				D2y 	= (-b - sqrt(Delta))/(2*a);
				D2x 	= d+e*D2y;
	 
			}
			else
			{
				Cx	= Bx;
				Cy	= By - length;
				
				CD	= length*tan(f);
				
				D1x	= Cx - CD;
				D1y = Cy;
				
				D2x	= Cx + CD;
				D2y = Cy;
				
				
			}
			local pointer_style	= spw::getproperty(::classId, "PointerStyle").index;
			
			if(pointer_style == 0)
			{
				
				local points= [point_construct(Cx,Cy),point_construct(D1x,D1y),point_construct(Bx,By),point_construct(D2x,D2y),point_construct(Cx,Cy)];
				dc.drawpolygon(5,points);
			}
			else
			{
				local points= [point_construct(D1x,D1y),point_construct(Bx,By),point_construct(Bx,By),point_construct(D2x,D2y)];
				dc.drawlines(4,points);
			}
		},

		method DisplayLines(dc)
		{
			local temp_list	= list_new();
			foreach(local l ,@Lines)//fixme
			{
				if(l.colour	!= "LineColour")
				{
					temp_list.push_back(l);
					continue;
				}
				local colour	= @GetColour(l.colour);
				local width		= spw::getproperty(::classId,l.width).value;
				local pen		= pen_construct(colour,width);
				dc.setpen(pen);
				local Points 	= l.Points;
				dc.drawspline(tablength(Points),Points);
				@DrawPointer(dc,Points[tablength(Points)-2],Points[tablength(Points)-1]);
			}
			foreach(l , temp_list)
			{
				local colour	= @GetColour(l.colour);
				local width		= spw::getproperty(::classId,l.width).value;
				local pen		= pen_construct(colour,width);
				dc.setpen(pen);
				local Points 	= l.Points;
				dc.drawspline(tablength(Points),Points);
				@DrawPointer(dc,Points[tablength(Points)-2],Points[tablength(Points)-1]);
			}
		},
		method PrepareGraph
		{
			@message	= nil;
		},
		method @operator()(Data)
		{
			@Nodes	= Data.Nodes;
			@Lines	= Data.Lines;
			@Other	= Data.Other;
		},
		method ViewProjects
		{
			currGeometry	= projectGeometry;
			projectGeometry.DisplayProjects();
			Renderer.PrepareGraph();
			window.refresh();
			//Renderer.Repaint(window);
		},
		method ViewScripts
		{
			Geometry.ApplyScripts();
			Renderer.PrepareGraph();
			window.refresh();
			//Renderer.Repaint(window);
		},
		method Init
		{
			window.setscrollrate(10, 10);
			window.scrollX	= 0;
			window.scrollY	= 0;
			window.userscale	= 1;

			window.Maindc	= clientdc_construct(window);
			window.connect(wx::EVT_PAINT, @Repaint);
			window.connect(wx::EVT_ERASE_BACKGROUND, @EraseBackground);
			window.progressdialog	= progressdialog_construct("Initializing", "== Start ==", 100, window,
				flags(PD_AUTO_HIDE, PD_SMOOTH, PD_APP_MODAL));
			@Print( "== Start ==", 20 );
		},
		method Print(text, percent)
		{
			@message	= text;
			if (arguments.total() == 2) {
				@loadPercent	= percent;
				
				if (@loadPercent == 100) {
					if(window.progressdialog){
					window.progressdialog.destroy();
					window.progressdialog	= nil;
					}return;
				}
			}
			if(not window.progressdialog)
				window.progressdialog	= progressdialog_construct("Initializing", "== Start ==", 100, window,
				flags(PD_AUTO_HIDE, PD_SMOOTH, PD_APP_MODAL));
			window.progressdialog.update(@loadPercent, @message);
	
		},
		method EraseBackground(window, ev)
		{
		},
		method Repaint(window)
		{
			local dc		= wx::bufferedpaintdc_construct(window, BUFFER_VIRTUAL_AREA);
			local colour	= @GetColour("BGColour");
			
			dc.setbackground(brush_construct(colour, "SOLID"));
			dc.clear();

			if (std::isnil(@message)) {
				dc.setfont(@GetFont("Font"));
				@DisplayOther(dc);
				@DisplayLines(dc);
				@DisplayNodes(dc);
			
			} 
			dc.setuserscale(window.userscale,window.userscale);
		}
	];
}
//~~~~ ~~~~ ~~~~ END OF RENDERER ~~~~ ~~~~ ~~~~ 
function CreateWindow(parent)
{
	window = wx::scrolledwindow_construct(wx::window_towxinstance(parent), wx::ID_ANY);
	return  window.tonativeinstance();
}

function Visualize {
	Renderer	= wxRenderer();
	Renderer.Init();
	
	local inter		= Interact1();
	inter.Init(window);
	
	Renderer.Print( "== Collect Scripts ==", 40 );
	Data		= CollectData();
	local g	= nil;
	local geometries	=[ G1 ,G2 ];
	local geometry_index	= spw::getproperty(::classId,"GeometryStyle").index;
	g	= geometries[geometry_index];

	Geometry	= g(Data);
	projectGeometry	= G1(Data);
	fileGeometry	= G1(Data);
	
	Geometry.ApplyScripts();
	currGeometry	= Geometry;
	
	Renderer(Data);
	Renderer.PrepareGraph();
	window.refresh();
	
}

//====================================PROPERTIES====================================//
function SetProperties { 
	
	function CreateColor(r, g, b) 
				{ return [ @class : "wxColour", @r : r, @g : g, @b : b ]; }
	local blue 			= CreateColor(0, 0, 255);
	local black 		= CreateColor(0, 0, 0);
	local purple 		= CreateColor(160, 32, 240);
	local white			= CreateColor(255, 255, 255);
	local red 			= CreateColor(221, 51, 55);
	local green 		= CreateColor(54 , 114 , 72);
	local light_green 	= CreateColor(102 , 198 , 74);
	local light_blue	= CreateColor(117 , 164 , 221);
	local gray			= CreateColor(224 , 224 , 224);
	local silver		= CreateColor(176 , 176 , 176);
	
	spw::addclassproperty(	
		::classId,
		"PointerStyle",		
		[
			{.class			: "EnumStringProperty"			},
			{.label			: "Pointer style"				},
			{.description	: "Decide the style of pointer"	},
			{.category		: "Rendering"					},
			{.options		: list_new("Arrow","Open Arrow")},
			{.index			: 0								}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"LevelsDistance",		
		[
			{.class			: "IntProperty"				},
			{.label			: "Distance of Levels"		},
			{.description	: "Distance between levels"	},
			{.category		: "Geometry"				},
			{.value			: 80						}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"GeometryStyle",		
		[
			{.class			: "EnumStringProperty"					},
			{.label			: "Geometry"							},
			{.description	: "Decide whigch geometry you want"		},
			{.category		: "Geometry"							},
			{.options		: list_new("Tree View","Project View")	},
			{.index			: 1										}
		]			
	);
	
	local defaultFont = [
						{.class : "wxFont"		},
						{.family: "SWISS"		},
						{.size 	: 8 			},
						{.weight: "NORMAL" 		},
						{.style : "NORMAL"		},
						{.underline	: false		},	
						{.facename	: "Arial"	}
					];
	spw::addclassproperty(
		::classId,
		"Font",		
		[
			{.class			: "FontProperty"		},
			{.label			: "Font"				},
			{.description	: "Index Font of nodes "},
			{.category		: "Rendering"			},
			{.value			: defaultFont			}
		]			
	);
	

	spw::addclassproperty(	
		::classId,
		"Node_BGcolour",		
		[
			{.class			: "ColorProperty"	},
			{.label			: "Colour of Nodes"	},
			{.description	: ""				},
			{.category		: "Rendering"		},
			{.value			: light_blue		}
		]			
	);
	
		spw::addclassproperty(	
		::classId,
		"SelectedBorderWidth",		
		[
			{.class			: "IntProperty"			},
			{.label			: "Selected node width"	},
			{.description	: ""					},
			{.category		: "Rendering"			},
			{.value			: 3						}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"DefaultBorderWidth",		
		[
			{.class			: "IntProperty"			},
			{.label			: "Default border width"},
			{.description	: ""					},
			{.category		: "Rendering"			},
			{.value			: 1						}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"NodeShape",		
		[
			{.class			: "EnumStringProperty"			},
			{.label			: "Node Shape"					},
			{.description	: "Select the shape of nodes"	},
			{.category		: "Rendering"					},
			{.options		: list_new("RoundRect","Rect")	},
			{.index			: 0								}
		]			
	);


	spw::addclassproperty(	
		::classId,
		"ShapeArc",		
		[
			{.class			: "IntProperty"				},
			{.label			: "Arc for RounRect shape"	},
			{.description	: "Arc for RounRect shape"	},
			{.category		: "Rendering"				},
			{.value			: 10						}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"LineColour",		
		[
			{.class			: "ColorProperty"		},
			{.label			: "Colour of Line"		},
			{.description	: "Default line Colour"	},
			{.category		: "Rendering"			},
			{.value			: black					}
		]			
	);

	//Selected_incoming_colour
	spw::addclassproperty(	
		::classId,
		"Selected_incoming_colour",		
		[
			{.class			: "ColorProperty"						},
			{.label			: "Incoming lines"						},
			{.description	: "Colour of selected incoming lines"	},
			{.category		: "Rendering"							},
			{.value			: green									}
		]			
	);
	//Selected_outgoing_colour	
	spw::addclassproperty(	
		::classId,
		"Selected_outgoing_colour",		
		[
			{.class			: "ColorProperty"						},
			{.label			: "Outgoing lines"						},
			{.description	: "Colour of selected outgoing lines"	},
			{.category		: "Rendering"							},
			{.value			: blue									}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"DefaultShapeColour",		
		[
			{.class			: "ColorProperty"					},
			{.label			: "Colour of shape"					},
			{.description	: "Default colour of shape border"	},
			{.category		: "Rendering"						},
			{.value			: green								}
		]			
	);

	//HorizonSpaceBetNodes
		spw::addclassproperty(	
		::classId,
		"SpaceBetNodes",		
		[
			{.class			: "IntProperty"			},
			{.label			: "Space Between Nodes"	},
			{.description	: ""					},
			{.category		: "Geometry"			},
			{.value			: 80					}
		]			
	);
	
	
	//RowLength
	spw::addclassproperty(	
		::classId,
		"PointerLength",		
		[
			{.class			: "IntProperty"						},
			{.label			: "Length of pointer"				},
			{.description	: "Select the length of the pointer"},
			{.category		: "Rendering"						},
			{.value			: 12								}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"PointerAngle",		
		[
			{.class			: "IntProperty"						},
			{.label			: "Angle of pointer "				},
			{.description	: "Select the angle of the pointer"	},
			{.category		: "Rendering"						},
			{.value			: 25								}
		]			
	);


	spw::addclassproperty(	
		::classId,
		"BGColour",		
		[
			{.class			: "ColorProperty"			},
			{.label			: "Colour of background"	},
			{.description	: ""						},
			{.category		: "Rendering"				},
			{.value			: gray						}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"SelectedNodeColour",		
		[
			{.class			: "ColorProperty"			},
			{.label			: "Colour of selected node"	},
			{.description	: "Colour of selected node"	},
			{.category		: "Rendering"				},
			{.value			: red						}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"PointerColour",		
		[
			{.class			: "ColorProperty"		},
			{.label			: "Pointer Colour"		},
			{.description	: "Colour of pointer"	},
			{.category		: "Rendering"			},
			{.value			: silver				}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"DefaultLineWidth",		
		[
			{.class			: "IntProperty"					},
			{.label			: "Line Width"					},
			{.description	: "the width of the normal line"},
			{.category		: "Rendering"					},
			{.value			: 1								}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"SelectedLineWidth",		
		[
			{.class			: "IntProperty"						},
			{.label			: "Selected Line Width"				},
			{.description	: "the width of the selected lines"	},
			{.category		: "Rendering"						},
			{.value			: 2									}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"ScriptsPerLevel",		
		[
			{.class			: "IntProperty"						},
			{.label			: "Scripts per Level"				},
			{.description	: "how many scripts fit in level"	},
			{.category		: "Geometry"						},
			{.value			: 6									}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"FactorX",		
		[
			{.class			: "IntRangeProperty"},
			{.label			: "FactorX (%)"		},
			{.category		: "Geometry"		},
			{.min			: 0					},
			{.max			: 100				},
			{.value			: 100				}
		]			
	);
	

	spw::addclassproperty(	
		::classId,
		"FactorY",		
		[
			{.class			: "IntRangeProperty"},
			{.label			: "FactorY (%)"		},
			{.category		: "Geometry"		},
			{.min			: 0					},
			{.max			: 100				},
			{.value			: 0				}
		]			
	);
	
		spw::addclassproperty(	
		::classId,
		"yOffset",		
		[
			{.class			: "IntProperty"			},
			{.label			: "yOffset"				},
			{.description	: "The vertical offset"	},
			{.category		: "Geometry"			},
			{.value			: 100					}
		]			
	);
}

//=================== END OF PROPERTIES ======================

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
    spw::registercomponent(classId);
    spw::setcomponentmetadata(
        classId, "Dependencies",
        "A utility for visualization of dependencies in the current workspace",
        "Koutsouroumpis Georgios <gkutsur@csd.uoc.gr>",
        "alpha"
    );

    spw::registerimage("dependency", spw::installationdir() + "/resources/dependency.png");

    spw::class_decl_required_member_function(classId, "Visualize", "uint (void)",
        "Visualize the dependency graph");
   spw::class_decl_required_member_command(
        [
            {.class            : "UserCommandDesc"	},
            {.class_id        : classId				},
            {.function_name    : "BuildDepsCmd"	},
            {.flags            : 7					},
            {.image            : "dependency"		}
        ],
        "/{200}Tools/{100}Dependency Graph",
        "Display the dependency graph for projects and scripts"
    );

	spw::class_impl_static_command(
			[
				{.class			: "UserCommandDesc"				},
				{.class_id		: classId						},
				{.function_name	: "ConfigureDependenciesGraph"	},
				{.flags			: 1								}
			],
			"/{2}Configure/Dependency Graph...",
			(function Configure {spw.components.Shell.ConfigureComponent(classId); }),
			"Configure the Dependency Graph"
		);
	
	SetProperties();
}


function ComponentAppliedChangedProperties(old, changed) {
	
	local commandList	= list_new();
	foreach(c,changed)
	{
		commandList.push_back(spw::getproperty(::classId,c).category);
	}
	local endList=listiter_new().setend(commandList);
	
	if(not find("GeometryStyle",changed).equal(listiter_new().setend(changed)))
	{
		local geometries	=[ G1 ,G2 ];
		local geometry_index	= spw::getproperty(::classId,"GeometryStyle").index;
		g	= geometries[geometry_index];

		Geometry	= g(Data);
		Geometry.ApplyScripts();
		Renderer.PrepareGraph();
	}
	else if( not find("Geometry",commandList).equal(endList))
	{
		Geometry.reapply();
		Renderer.PrepareGraph();
	}
	window.refresh();
	
}
//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
    spw::inst_impl_required_member_function(classId, "Visualize", Visualize);
    spw::inst_impl_required_member_command(classId, "BuildDepsCmd", Visualize);
}

//-----------------------------------------------------------------------

onevent Destructor {}

//-----------------------------------------------------------------------