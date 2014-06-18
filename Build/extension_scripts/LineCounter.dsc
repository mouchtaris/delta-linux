using std;
using #sparrowlib;
using wx;
spw  = sparrowlib::sparrow();
assert(spw);
nl = "\n";
const classId = "LineCounter";

(function {if (not static dll) dll = std::dllimportdeltalib(wx::DLL);})();
initallimagehandlers();



//====================================PROPERTIES====================================//
function SetProperties { 
	

	function CreateColor(r, g, b) 
				{ return [ @class : "wxColour", @r : r, @g : g, @b : b ]; }
	local blue 		= CreateColor(0, 0, 255);
	local black 	= CreateColor(0, 0, 0);
	local purple 	= CreateColor(160, 32, 240);
	local white		= CreateColor(255, 255, 255);
	local red 		= CreateColor(255, 0, 0);
	local green 	= CreateColor(0 , 255 , 0);
	local light_blue= CreateColor(117 , 164 , 221);
	local gray		= CreateColor(230 , 230 , 230);
	spw::addclassproperty(	
		::classId,
		"LevelsDistance",		
		[
			{.class			: "IntProperty"			},
			{.label			: "Distance of Levels"	},
			{.description	: "Distance between levels"					},
			{.category		: "General"				},
			{.value			: 120					}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"DistanceStep",		
		[
			{.class			: "IntProperty"		},
			{.label			: "Increase-Decrease Distance"		},
			{.description	: "step for moving nodes on increase or decrease button"},
			{.category		: "General"			},
			{.value			: 50				}
		]			
	);
	local defaultFont = [
						{.class : "wxFont"	},
						{.family : "SWISS"	},
						{.size : 8 			},
						{.weight : "NORMAL" },
						{.style : "NORMAL"	}
					];
	spw::addclassproperty(	
		::classId,
		"Font",		
		[
			{.class			: "FontProperty"		},
			{.label			: "Font"				},
			{.description	: "Index Font of nodes "},
			{.category		: "Default"				},
			{.value			: defaultFont			}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"MessageFont",		
		[
			{.class			: "FontProperty"		},
			{.label			: "Message Font"				},
			{.description	: "Display messages font "},
			{.category		: "Default"				},
			{.value			: defaultFont			}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"Node_BGcolour",		
		[
			{.class			: "ColorProperty"		},
			{.label			: "Colour of Nodes"		},
			{.description	: ""					},
			{.category		: "General"				},
			{.value			: light_blue					}
		]			
	);
	
		spw::addclassproperty(	
		::classId,
		"SelectedBorderWidth",		
		[
			{.class			: "IntProperty"		},
			{.label			: "Shift Space"		},
			{.description	: ""				},
			{.category		: "General"			},
			{.value			: 3					}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"DefaultBorderWidth",		
		[
			{.class			: "IntProperty"		},
			{.label			: "Shift Space"		},
			{.description	: ""				},
			{.category		: "General"			},
			{.value			: 1					}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"MessageBGColour",		
		[
			{.class			: "ColorProperty"		},
			{.label			: "Background colour for messages"		},
			{.description	: "Background colour for messages"					},
			{.category		: "General"				},
			{.value			: green					}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"NodeShape",		
		[
			{.class			: "EnumStringProperty"						},
			{.label			: "Node Shape"								},
			{.description	: "Select the shape of nodes"				},
			{.category		: "General"									},
			{.options		: list_new("RoundRect","Rect","Ellipse")	},
			{.index			: 2											}
		]			
	);



	spw::addclassproperty(	
		::classId,
		"ShapeArc",		
		[
			{.class			: "IntProperty"								},
			{.label			: "Arc for RounRect shape"					},
			{.description	: "Arc for RounRect shape"										},
			{.category		: "General"									},
			{.value			: 10										}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"LineColour",		
		[
			{.class			: "ColorProperty"							},
			{.label			: "Colour of Lines"							},
			{.description	: "Default line Colour"										},
			{.category		: "General"									},
			{.value			: black										}
		]			
	);

	//Selected_incoming_colour
	spw::addclassproperty(	
		::classId,
		"Selected_incoming_colour",		
		[
			{.class			: "ColorProperty"								},
			{.label			: "Incoming lines"								},
			{.description	: "Colour of selected incoming lines"			},
			{.category		: "General"										},
			{.value			: blue											}
		]			
	);
	//Selected_outgoing_colour	
	spw::addclassproperty(	
		::classId,
		"Selected_outgoing_colour",		
		[
			{.class			: "ColorProperty"								},
			{.label			: "Outgoing lines"								},
			{.description	: "Colour of selected outgoing lines"			},
			{.category		: "General"										},
			{.value			: white										}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"DefaultShapeColour",		
		[
			{.class			: "ColorProperty"								},
			{.label			: "Colour of shape"								},
			{.description	: "Default colour of shape"			},
			{.category		: "General"										},
			{.value			: black											}
		]			
	);

	//SpaceBetNodes
		spw::addclassproperty(	
		::classId,
		"SpaceBetNodes",		
		[
			{.class			: "IntProperty"								},
			{.label			: "Space Between Nodes"						},
			{.description	: ""										},
			{.category		: "General"									},
			{.value			: 200										}
		]			
	);
	//RowLength
	spw::addclassproperty(	
		::classId,
		"PointerLength",		
		[
			{.class			: "IntProperty"								},
			{.label			: "Length of pointer"								},
			{.description	: "Select the length of the pointer"			},
			{.category		: "General"										},
			{.value			: 12											}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"PointerAngle",		
		[
			{.class			: "IntProperty"								},
			{.label			: "Angle of pointer "								},
			{.description	: "Select the angle of the pointer"			},
			{.category		: "General"										},
			{.value			: 30											}
		]			
	);


	spw::addclassproperty(	
		::classId,
		"BGColour",		
		[
			{.class			: "ColorProperty"			},
			{.label			: "Colour of background"	},
			{.description	: ""						},
			{.category		: "General"					},
			{.value			: gray						}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"LineColour",		
		[
			{.class			: "ColorProperty"		},
			{.label			: "Colour of Line"		},
			{.description	: "Colour of Line"					},
			{.category		: "General"				},
			{.value			: black					}
		]			
	);
	
	spw::addclassproperty(	
		::classId,
		"SelectedNodeColour",		
		[
			{.class			: "ColorProperty"			},
			{.label			: "Colour of selected node"	},
			{.description	: "Colour of selected node"						},
			{.category		: "General"					},
			{.value			: purple						}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"DefaultLineWidth",		
		[
			{.class			: "IntProperty"						},
			{.label			: "Line Width"				},
			{.description	: "the width of the normal line"	},
			{.category		: "General"							},
			{.value			: 0									}
		]			
	);

	//direct
	spw::addclassproperty(	
		::classId,
		"Direct",		
		[
			{.class			: "BoolProperty"					},
			{.label			: "Direct Lines"					},
			{.description	: "Select if the lines are direct"	},
			{.category		: "General"							},
			{.value			: true								}
		]			
	);
	spw::addclassproperty(	
		::classId,
		"ScriptsPerLevel",		
		[
			{.class			: "IntProperty"						},
			{.label			: "Scripts per Level"				},
			{.description	: "how many scripts fit in level"	},
			{.category		: "General"							},
			{.value			: 4									}
		]			
	);
}
//=================== END OF PROPERTIES ======================



function newNode(Nodes)
{	
	local tmp =
	[
		@name		: 0	,
		@x		: 0	,
		@y		: 0	,
		@width	: 0	,
		@height	: 0	,
		@script	: 0 ,
		@BGcolour	: "Node_BGcolour" ,
		@Level	: 0			,
		@inLines	: list_new(),
		@outLines	: list_new(),
		@BorderWidth	: spw::getproperty(::classId,"DefaultBorderWidth").value,
		@Shape_Colour: "DefaultShapeColour",
		@selected	: false,
		@file 	: 0,

		method (name,x,y,width,height)
		{
			@name	= name;
			@x		= x;
			@y		= y;
			@width	= width;
			@height	= height;
			spw::print("Dhmiourgw ton komvo tou ",@name,nl);
	
		},
		method reset()
		{
			spw::print("Reset node :",@name,nl);
			@BorderWidth	= spw::getproperty(::classId,"DefaultBorderWidth").value;
			
			@Shape_Colour	= "DefaultShapeColour";
			@selected		= false;
			foreach(local l,@inLines)
			{
				l.from.Shape_Colour	= "DefaultShapeColour";
				l.from.BorderWidth	= spw::getproperty(::classId,"DefaultBorderWidth").value;
				l.reset();
			}
			foreach(local l,@outLines)
			{
				l.to.Shape_Colour	= "DefaultShapeColour";
				l.to.BorderWidth	= spw::getproperty(::classId,"DefaultBorderWidth").value;
				l.reset();
			}
			
		},
		method move(dx,dy)
		{
			@x	+= dx;
			@y	+= dy;
		},
		method stopMoving(possibleLevel,userscale)
		{
			@changeLevel(possibleLevel(@y,userscale));
		},
		method highlight()
		{
			spw::print("HighLight\n");
			@BorderWidth	= spw::getproperty(::classId,"SelectedBorderWidth").value;
			@Shape_Colour	= "SelectedNodeColour";
			spw::print("Pathsa panw sto ",@name,nl);
			local inColour	= "Selected_incoming_colour";
			foreach(line,@inLines)
			{
				line.colour				= 
				line.from.Shape_Colour	= inColour;
				line.from.BorderWidth	= spw::getproperty(::classId,"SelectedBorderWidth").value;
			}
				
			local outColour	= "Selected_outgoing_colour";
			foreach(line,@outLines)
			{
				line.colour				= 
				line.to.Shape_Colour	= outColour;
				line.to.BorderWidth		= spw::getproperty(::classId,"SelectedBorderWidth").value;
			}

		},
		method setX(num){@x=num;},
        method setY(num){@y=num;},
		method setLevel(l)
		{
			@level=l; 
			spw::print("+++++ Vazw to ",@name," sto level :",@level.num,nl);
			@level.add(self);
		}, 
		method changeLevel(l)
		{
			@level.Scripts.remove(self);
			spw::print("O komvos ",@name," brisketai arxika sto level ",@level.num,nl);
			l.put(self);
			@y	= @level.getY();
			@reconnect();
			
		},
		method reconnect()
		{

			foreach(local l,@outLines)
			{
				l.Points	= @GetPath(self,l.to);
			}

			foreach(local l,@inLines)
			{
				l.Points	= @GetPath(l.from,self);
			}
			
			
		},
		method getCx(){
				return (@x+@width/2);
		},//Center x
        method getCy()	{	return (@getY()+@height/2);	},//Center y
		method getX()	{	return @x;					},
        method getY()	{	return @y;					},
		method getLevel(){	return @level;				}
			
	];
	Nodes.push_back(tmp);
	return tmp;
}




//-------------------- SCRIPT ---------------------

function newFile()
{
	return [
		@type			: "" ,
		@name			: 0,
		
        @id				: 0,
        @project		: 0,		
		@dependencies	: list_new(),
        @neighbours		: list_new() ,
        @ownList		: list_new() ,
        @visited		: false ,
        @marked			: false ,
		@CurrDependencies	: list_new(),
		@CurrNeighbours	: list_new(),
		@script_deps	: list_new(),
        @temp_level		: -1 ,
        @parents		: list_new() ,
		@unique			: true ,
		@node			: 0 ,
		method Script(script,project)
			{
				@type		= "script";
				@id			= script;
				@project	= project;
				@name		= script.GetName();
				spw::print("To ",@name," einai script!!\n");
				spw::print("me project:",@project.name);
			},
		method Project(project)
			{
				@type	= "project";
				@id		= project;
				@name	= project.GetName();
				spw::print("To ",@name," einai project!!\n");

			},
		method GetFName()//FullName
		{
			local project	= "";
			if(not @unique)
				project	 	+= @project.name+"/";
			return project+@name;
		},
        method init()
        {
			@visited	= false;
			@marked		= false;
			@temp_level	= -1;
			@ownList	= list_new();
        },
		method visit(){@visited	= true;},
		method getNode(Nodes)
		{
			if(not @node)
			{
				spw::print("Ftiaxnw ton komvo tou ",@name,nl);
				@node	= newNode(Nodes);
				@node.file	= self; 
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

function newLevel(level_num)
{
    return [
			{"Scripts":list_new()},
            {"num":level_num},
			{"size":0},
			method add(s)
			{
				@size	+= s.width;
				@Scripts.push_back(s);
			},
            method addLevelFrom(l,Levels)
			{
				local next=find(self,Levels).fwd();
                for(i=@num+1;i<=@num+l;i++)
					Levels.insert(next,newLevel(i));
                    for(;not next.equal(listiter_new().setend(Levels));next.fwd())
                    {
						next.getval().num+=l;
                    }
                    return find(self,Levels).fwd().getval();
            },
            method getScripts(){return @Scripts;},
            method getY()
			{
				spw::print();
				local LevelsDistance	=	spw::getproperty(::classId, "LevelsDistance").value;
				return @num*LevelsDistance+LevelsDistance/2;
			},
            method getScriptNum(script)
			{
                return find(script,@Scripts).getcounter();
            },
            method LookInLevel(userscale ,x ,y)
            {
				foreach(s,@Scripts)
                {
					if(userscale*s.x<=x and userscale*s.x+ userscale*s.width>=x)
//                        if(window.userscale*s.getY()<=y and window.userscale*s.getY()+window.userscale*s.height>=y)
						return s;
                }
                return nil;
            },
			method put(node)
			{
				node.level	= self;
				@Scripts.push_back(node);

			}
    ];
}

//-------------------------------------------------

//------------------- DATA --------------------



function newLine(Points,from,to,Lines)
{
	local tmp= [
				{"Points"	: Points		},
				{"from"		: from			},
				{"to"		: to			},
				{"colour"	: "LineColour"	},
				{"width"	: spw::getproperty(::classId,"DefaultLineWidth").value	},
				method reset()
				{
					@width	= spw::getproperty(::classId,"DefaultLineWidth").value;
					@colour	= "LineColour";
					//@from.reset();
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

function collectData()
{
	
	function AddScript()
	{
		return 
		[
			@Scripts	: [],
			@Projects	: [],
			@Lines		: list_new(),
			@Nodes		: list_new(),
			method watch_dependencies(file,dependencies)
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
						local file	= newFile();
						file.Project(project);
						@Projects[project.serial]	= file;
						spw::print("Project :",@Projects[project.serial].name,nl);
					}
	   
					if(@Scripts[script.serial]==nil)
					{
						spw::print("To ",script.GetName()," prepei na mpei san script",nl);
						local file	= newFile();
						file.Script(script,@Projects[project.serial]);
						@Scripts[script.serial]	= file;
						//spw::print("Script :",@Scripts[script.serial].name,nl);
						//spw::print("\t me project :",@Scripts[script.serial].project.name,nl);
						
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
					@watch_dependencies(@Scripts[script.serial],dependencies);
	   
				}
			}
		];
		
	}
	function for_each(t,f){	foreach(x,t) f(x,nil); return f;}
	local scripts = spw.components.ProjectManager.GetResources("Script");
	return for_each(scripts,AddScript());
}
//------------- END OF DATA ----------------------

//------------- GEOMETRY -------------------------

//				GEOMETRY2


function distance(A,B)
{
    return sqrt(sqr(B.gety()-A.gety())+sqr(B.getx()-A.getx()));
}
function getcloser(Min,A,B)
{
	
    if (Min==nil or (distance(A,B)<distance(A,Min)))
        return point_construct(B);
    else
        return point_construct(Min);
}
function between(C,A,B)
{
    return (C>=A and C<=B);
}

function CollisionPoint(A,B)
{
    local p=point_construct(0,0);
    local dy = B.getCy()-A.gety();
    local dx = B.getCx()-A.getx();
    local l=0;
   
    if(dx!=0)
    {
        if(A.gety()==B.getCy())
        {
            if(A.getx()>B.getCx())
            {
                p.sety(A.gety());
                p.setx(B.getX()+B.width);
            }
            else
            {
                p.sety(A.gety());
                p.setx(B.getX());
            }
            return p;
        }
        l=dy/dx;
        local pA=point_construct(A.getx(),A.gety());
       
        local Min=nil;
        local tempP;
        if(between(l*(B.getX()-B.getCx())+B.getCy(),B.getY(),B.getY()+B.height)) //left
        {
            tempP=point_construct(B.getX(),l*(B.getX()-B.getCx())+B.getCy());
            Min=getcloser(Min,pA,tempP);
        }
        if(between(l*(B.getX()+B.width-B.getCx())+B.getCy(),B.getY(),B.getY()+B.height)) //right
        {
            tempP=point_construct(B.getX()+B.width,l*(B.getX()+B.width-B.getCx())+B.getCy());
            Min=getcloser(Min,pA,tempP);
        }
        if(between((B.getY()-B.getCy())/l+B.getCx(),B.getX(),B.getX()+B.width))//up
        {
            tempP=point_construct((B.getY()-B.getCy())/l+B.getCx(),B.getY());
            Min=getcloser(Min,pA,tempP);
        }
        if(between((B.getY()+B.height-B.getCy())/l+B.getCx(),B.getX(),B.getX()+B.width)) //down
        {
            tempP=point_construct((B.getY()+B.height-B.getCy())/l+B.getCx(),B.getY()+B.height);
            Min=getcloser(Min,pA,tempP);
        }
        return Min;
		
    }
    else
    {
		local NodeHeight	=	B.height;
        if(A.gety()>B.getCy()){ p.setx(A.getx()); p.sety(B.getY()+NodeHeight);}
        else { p.setx(A.getx()); p.sety(B.getY()); }
    }
    return p;
   
}
function maxLevel(Levels)
{
	if(Levels.total()==0)
		return;
	Max = Levels.front();
	foreach(l,Levels)
	{
		if(l.size>Max.size)
			Max=l;
	}
	return Max;
}

function g1(Data)
{
	
	function array2list(array)
	{
		if(typeof(array)!="Table" and typeof(array)!="Object")
			return;
		local list=list_new();
		foreach (a,array)
		{
			list.push_back(a);
		}
	   
		return list;
	}
	function sort(List)
	{
		local i=listiter_new();
		for (i=i.setbegin(List);(not (i.checkend(List)));i=i.fwd())
		{
		   
			local script1=i.getval();
			//spw::print("Koitaw to :",script1.script.GetName(),nl);
			local j=listiter_new();
			for (j.assign(i);(not (j.checkend(List)));j=j.fwd())
			{
				local script2=j.getval();
			   
				if(script1.neighbours.total()>script2.neighbours.total())
				{
					//spw::print("Allazw to ",script1.name,"(",script1.dependencies.total(),") me to ",script2.name,"(",script2.dependencies.total(),")",nl);
					temp=i.getval();
					i.setval(j.getval());
					j.setval(temp);
					script1=i.getval();
				}
			}
		}
		local j_temp=listiter_new();
		for (i=i.setbegin(List);(not (i.checkend(List)));i=i.fwd())
		{
			local script1=i.getval();
			local j=listiter_new();
			j_temp.assign(i);
			for (j.assign(j_temp);( (not (j.checkend(List))) and (j.getval().dependencies.total()==i.getval().dependencies.total()));j=j.fwd())
			{
				local script2=j.getval();
				if(not find(script1,script2.dependencies).equal(listiter_new().setend(script2.dependencies)) or script1.dependencies.total()<script2.dependencies.total())
				{
					//spw::print("Allazw to ",script1.name,"(",script1.neighbours.total(),") me to ",script2.name,"(",script2.neighbours.total(),")",nl);
				   
					temp=i.getval();
					i.setval(j.getval());
					j.setval(temp);
					script1=i.getval();
				}           
			}
		}
	   
		return List;
	}
	local Scripts	=
	Data.Scripts	= sort(array2list(Data.Scripts));
	local Projects	=
	Data.Projects	= sort(array2list(Data.Projects));
	local Levels	= list_new();
	local View		= "";
    local CurrList	= list_new();
	local Singles	= list_new();
    local Commons	= list_new();
    local Roots		= list_new();
	local Parents	= list_new();
	local parent	= 0;
	local Lines		= Data.Lines;
	local Nodes		= Data.Nodes;
	local SpaceBetNodes	= 0;
	
	
	function neighbours(s)
	{
		list = list_new();
		foreach(o,s.neighbours)
			if(not find(o,CurrList).equal(listiter_new().setend(CurrList)))
				list.push_back(o);
		return list;
	}
       
	function dependencies(s)
	{
		list = list_new();
		foreach(o,s.dependencies)
			if(not find(o,CurrList).equal(listiter_new().setend(CurrList)))
				list.push_back(o);
		//s.CurrDependencies = list;
		return list;
	}	
	function init(obj)
	{
		obj.temp_level	= -1;
		obj.marked	= false;
		obj.visited	= false;
		obj.ownList	= list_new();
		obj.CurrDependencies	= dependencies(obj);
		spw::print(obj.name,".dependencies	= ",obj.dependencies.total(),nl);
		spw::print(obj.name,".CurrDependencies	= ",obj.CurrDependencies.total(),nl);
		
		obj.CurrNeighbours		= neighbours(obj);
		spw::print(obj.name,".neighbours	= ",obj.neighbours.total(),nl);
		spw::print(obj.name,".CurrNeighbours	= ",obj.CurrNeighbours.total(),nl);
		
		local node	= obj.getNode(Nodes);
		node.name	= obj.GetFName()+"{users:"+obj.CurrDependencies.total()+" , used:"+obj.CurrNeighbours.total()+" } ";
		
		node.reset();
	}
	function list_copy(List)
	{
		new_list = list_new();
		foreach(l,List)
			new_list.push_back(l);
		return new_list;
	}
	
	function UpdateOwnList(s)
	{
		foreach(o,s.ownList)
		{
			o.temp_level=s.temp_level+1;
			UpdateOwnList(o);
		}
	}
	function findObstacles(p1,B,temp_array,lev,Levels)
	{
		local NodeHeight	= 20;
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
					if(distance(p1,point_construct(x1,y1))<distance(p1,point_construct(x2,y2)))
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
			temp_array=findObstacles(point_construct(x,y),B,temp_array,find(lev,Levels).fwd().getval(),Levels);
		}
		else
		{
			temp_array=findObstacles(point_construct(x,y),B,temp_array,find(lev,Levels).bwd().getval(),Levels);
		}
		return temp_array;
		
	}
	function putRoots()
	{
		foreach(s,Roots)
		{
			local node	= s.node;
			node.setLevel(Levels.front());
			node.visited	= true;
			node.marked		= true;
			foreach(o,s.ownList)
			{
				o.temp_level = s.temp_level+1;
				UpdateOwnList(o);
			}
		}
	}

	function checkCommons()
	{
		if(Commons.total()==0)
			return false;
		foreach(c,Commons)
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
				UpdateOwnList(c);
				Commons.remove(c);
				return true;
			}
                
		}
		return false ;
	}
	    /*
        AdditionalDeps of script s returns all children-scripts that are not visited
        */
    function additionalDeps(s)
	{
		foreach(local o,s.ownList)
        {
			if(o.visited)
				continue;
				
            o.visited	= true;
				
            local l;
			local sNode	= s.node;
			local oNode	= o.node;
            if(sNode.getLevel().num==Levels.total()-1)
				l=sNode.getLevel().addLevelFrom(1,Levels);
			else
				l=find(sNode.getLevel(),Levels).fwd().getval();

			oNode.setLevel(l);
			additionalDeps(o);
		}
	}
	
    function map()
	{

        
        if(Roots.total()==0)
        {
			local node	= CurrList.front().node;
			node.setLevel(Levels.front());
			node.visited	= true;
		}

        foreach(obj,CurrList)
        {
            //obj.dependencies=array2list(obj.dependencies);
			/*if(not find(obj,Roots).equal(listiter_new().setend(Roots)))
            {
				obj.visit();
				obj.setLevel(Levels.front());
				Roots.remove(obj);
			}*/
            additionalDeps(obj);   
		}
	}
	
return [
	@Nodes 	: Nodes,
	@Lines 	: Lines,
	
	method ConnectScripts(A,B)
	{
		local Points	= @GetPath(A,B);
		local line		= newLine(Points,A,B,Lines);
		//Lines.push_back(Line_Pointer);
		A.outLines.push_back(line);
		B.inLines.push_back(line);		
	},
	method GetPath(A,B)
	{
		p1		= point_construct(A.getCx(),A.getCy());
		local Points,p2;
		p2=point_construct(B.getCx(),B.getCy());

		local direct	=	spw::getproperty(::classId, "Direct").value;
		if(direct)
		{
			p2 			= CollisionPoint(p1,B);
			Points 		= [p1,point_construct(B.getCx(),B.getCy())];
		}
		else
		{
			if(A.getLevel().num>B.getLevel().num)
				Points	= findObstacles(p1,B,[],find(A.getLevel(),Levels).bwd().getval(),Levels);
			else
				Points	= findObstacles(p1,B,[],find(A.getLevel(),Levels).fwd().getval(),Levels);
			p2 		= CollisionPoint(tabget(Points , tablength(Points)-2),B);
		}	
		tabset(Points , tablength(Points)-1,p2);
		return Points;
	},
	method SetScripts(width)
	{
		spw::print("Width	= ",width,nl);
		foreach(l,Levels)
		{
			Space	=	(width-l.size)/(l.Scripts.total()+1);
			spw::print("Gia to LEvel ",l.num," ta kena exoun megethos :",Space," epishs exei ",l.Scripts.total()," scripts",nl);
			start	= Space;
			foreach(s,l.Scripts)
			{
				local x	= start;
				local y	= l.getY();
				local width		= @getWidth(s.name);
				local height	= @getHeight(s.name);
				spw::print(s.name,":",nl);
				spw::print("\t width	= ",width,nl);
				spw::print("\t height	= ",height,nl);
				s.x	= x;
				s.y	= y;
				s.width		= width;
				s.height	= height;
				s.GetPath	= @GetPath;
				start	+= width + Space;
			}
		}
	},

	method apply(List)
	{
		spw::print("apply List",nl);
			
		if(List.type and List.type	== "script")
		{
			CurrList	= sort(list_copy(List.dependencies));
			CurrList.push_front(List);
		}
		else if(List.type and List.type	== "project")
		{
			CurrList	= sort(list_copy(List.script_deps));
			CurrList.push_front(List);
		}

		else
			CurrList=sort(list_copy(List));

		spw::print("LOAD",nl);
		
		if(CurrList.total()==0)
			return;
			
		SpaceBetNodes	 = spw::getproperty(::classId, "SpaceBetNodes").value;
	//sos			loading(@frame.scrolledwindow, 1);
		Nodes.clear();
		Lines.clear();
		
		if(Nodes.total()==0)
			spw::print("Ta nodes den exoun kanena kombo!\n");
		else
			spw::print("----------------Exei diafora h lista me ta nodes\n");
		
		Commons=list_new();
		Singles=list_new();
		Roots=list_new();
		Levels=list_new();
	//sos			local dc		= clientdc_construct(frame.scrolledwindow);
	//sos				dc.setfont(config.Font);
		
		
		foreach(obj,CurrList)
			init(obj);
		foreach(obj,CurrList)
		{
			local end	= listiter_new().setend(Commons);
			local neighs= obj.CurrNeighbours;
			local deps	= obj.CurrDependencies;
			spw::print("Deps ",obj.name,":",nl);
			foreach(s,deps)
				spw::print("\t",s.name,nl);
				
/*Commons*/	if( neighs.total()>1 and (find(obj,Commons).equal(end)==true))
			{
				spw::print(obj.name,"->Commons\n");
				Commons.push_back(obj);
			}
/*OwnList*/	else if(neighs.total()==1 and not find(neighs.front(),CurrList).equal(listiter_new().setend(CurrList)) )//if(file.dependencies==0 and file.neighbours==0)
			{
				spw::print(obj.name,"->ownList\n");
				obj.marked=true;
				neighs.front().ownList.push_back(obj);//this file belongs to the unique neighbour
			}
/*Singles*/	else if(neighs.total()==0 and deps.total()==0)
			{
				spw::print(obj.name,"->Singles\n");
				obj.marked=true;
				Singles.push_back(obj);
				CurrList.remove(obj);
				Nodes.remove(obj.node);
				obj.node	= 0;
				obj.temp_level=0;
			}
/*Roots*/	else if(neighs.total()==0 )
			{
				spw::print(obj.name,"->Roots\n");
				Roots.push_back(obj);
				obj.temp_level=0;
				obj.marked=true;
			}
		}
//sos				foreach(obj,@CurrList)
//sos					obj.WidthSize = dc.gettextextent(obj.GetFName()).getwidth();
				
		if(CurrList.total()==0)
		{
			@warning("There are no dependencies!\n");
			return;
		}
		spw::print("==========ROOTS=========\n");
		foreach(s,Roots)
			spw::print(s.name,nl);
		spw::print("==========SINGLES=========\n");
		foreach(s,Singles)
			spw::print(s.name,nl);	
		spw::print("-------------------------\n");
		Levels.push_back(newLevel(0));
		spw::print("Levels.total()	=	 ",Levels.total(),nl);
		putRoots();
		
		while(checkCommons());
		
//sos				loading(@frame.scrolledwindow, 2);
			
		map();
		
		@FitSpace();
		spw::print("END OF LOAD!\n");
	},
	method detectLevel(x,y,userscale)
	{
		foreach(l,Levels)
		{
			local NodeHeight	= @getHeight("T");//fixme
			if(y<=userscale*l.getY()+userscale*NodeHeight and y>=userscale*l.getY())
			{
				return l;
			}
		}
		return nil;
	},
	method possibleLevel(y,userscale)
	{
		spw::print("possibleLevel",nl);
		
	//		return Levels.back();
		local ClosestLevel	= Levels.front();
		function distance(a,b)
		{
			return abs(b-a);
		}
		
		foreach(l,Levels)
		{
			if(distance(y,l.getY())<distance(y,ClosestLevel.getY()))
				ClosestLevel	= l;
		}
		return ClosestLevel;
	/*		if(abs(y-Levels.front().getY())<abs(y-Levels.back().getY()))
				return Levels.front();
			else
				return Levels.back();
*/	},
	method DetectObject(x,y,userscale)
	{
		local l	= @detectLevel(x,y,userscale);
		if(l and (result = l.LookInLevel(userscale,x,y))!=nil)
			return result;
		return nil;
	},
	method FitSpace()
	{
		local l = maxLevel(Levels);
		//local 	SpaceBetNodes	=	spw::getproperty(::classId, "SpaceBetNodes").value;
		LevelWidth	= l.size+((l.Scripts.total()-1)*SpaceBetNodes);
		@SetScripts(LevelWidth);
//sos				loading(@frame.scrolledwindow, 3);
		spw::print("=== Connect ===\n");
		foreach(s,CurrList)
		{
			
			s.node.outLines = list_new();
			s.node.inLines 	= list_new();
		}
		
		foreach (s,CurrList)
		{
			foreach(o, s.CurrDependencies)
			{
				@ConnectScripts(s.node,o.node);
			}
		}
//so			loading(@frame.scrolledwindow, 4);
		spw::print("=== Ready ===\n");
//sos			loading(@frame.scrolledwindow, nil);
		width = LevelWidth;
		local 	NodeHeight	=	20;
		height =Levels.back().getY()+NodeHeight;
//sos				@frame.scrolledwindow.setvirtualsize(width,height);
//sos				@frame.scrolledwindow.scroll((frame.scrolledwindow.getscrollrange("HORIZONTAL") - frame.scrolledwindow.getscrollthumb("HORIZONTAL")) / 2, 0);

//sos				@frame.scrolledwindow.refresh();
	},
	method displayProjects()
	{
		spw::print("H lista me ta projects exei ta exhs:\n");
		foreach(p,Projects)
		{
			spw::print("\t",p.name,nl);
			foreach(d,p.dependencies)
			{
				spw::print("\t\t",d.name,nl);
			}
		}
		@apply(Projects);
	},
	method displayScripts()
	{
		
		@apply(Scripts);
	},
	method IncreaseSpace()
	{
		spw::print("Increase SPace\n");
		
		//spw::getproperty(::classId, "SpaceBetNodes").value	=	spw::getproperty(::classId, "SpaceBetNodes").value +100;
		local l = maxLevel(Levels);
		local DistanceStep	= spw::getproperty(::classId,"DistanceStep").value;
		SpaceBetNodes	+= DistanceStep;
		LevelWidth	= l.size+((l.Scripts.total()-1)*SpaceBetNodes);
		@SetScripts(LevelWidth);
		foreach(l,Levels)
		{
			foreach(node,l.Scripts)
			{
				node.reconnect();
				spw::print("reconnect:",node.name,nl);
			}
		}
		
	},
	method DecreaseSpace()
	{
		spw::print("Increase SPace\n");
		local l = maxLevel(Levels);
		local DistanceStep	= spw::getproperty(::classId,"DistanceStep").value;
		SpaceBetNodes	-= DistanceStep;
		LevelWidth	= l.size+((l.Scripts.total()-1)*SpaceBetNodes);
		@SetScripts(LevelWidth);
		foreach(l,Levels)
		{
			foreach(node,l.Scripts)
			{
				node.reconnect();
			}
		}
	}

	];
}


function g2(Data)
{
	local Scripts	= Data.Scripts;
	local Projects	= Data.Projects;
	local Nodes		= Data.Nodes;
	local Lines		= Data.Lines;
	local Levels	= list_new();
	local NodeHeight	= 20;
	function findObstacles(p1,B,temp_array,lev,Levels)
	{
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
					if(distance(p1,point_construct(x1,y1))<distance(p1,point_construct(x2,y2)))
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
			temp_array=findObstacles(point_construct(x,y),B,temp_array,find(lev,Levels).fwd().getval(),Levels);
		}
		else
		{
			temp_array=findObstacles(point_construct(x,y),B,temp_array,find(lev,Levels).bwd().getval(),Levels);
		}
		return temp_array;
		
	}
	local SpaceBetNodes	= 0;
return [
	@Nodes 	: Nodes,
	@Lines 	: Lines,
	method ConnectScripts(A,B)
	{
		local Points	= @GetPath(A,B);
		local line		= newLine(Points,A,B,Lines);
		//Lines.push_back(Line_Pointer);
		A.outLines.push_back(line);
		B.inLines.push_back(line);		
	},
	method GetPath(A,B)
	{
		p1		= point_construct(A.getCx(),A.getCy());
		local Points,p2;
		p2=point_construct(B.getCx(),B.getCy());

		local direct	=	spw::getproperty(::classId, "Direct").value;
		if(direct)
		{
			p2 			= CollisionPoint(p1,B);
			Points 		= [p1,point_construct(B.getCx(),B.getCy())];
		}
		else
		{
			if(A.getLevel().num>B.getLevel().num)
				Points	= findObstacles(p1,B,[],find(A.getLevel(),Levels).bwd().getval(),Levels);
			else
				Points	= findObstacles(p1,B,[],find(A.getLevel(),Levels).fwd().getval(),Levels);
			p2 		= CollisionPoint(tabget(Points , tablength(Points)-2),B);
		}	
		tabset(Points , tablength(Points)-1,p2);
		return Points;
	},
	method SetScripts(width)
	{
		spw::print("Width	= ",width,nl);
		foreach(l,Levels)
		{
			Space	=	(width-l.size)/(l.Scripts.total()+1);
			spw::print("Gia to LEvel ",l.num," ta kena exoun megethos :",Space,nl);
			start	= Space;
			foreach(s,l.Scripts)
			{
				local x	= start;
				local y	= l.getY();
				local width	= @getWidth(s.name);
				s.x	= x;
				s.y	= y;
				start	+= width + Space;
			}
		}
	},
	method detectLevel(x,y,userscale)
	{
		foreach(l,Levels)
		{
			local NodeHeight	= @getHeight("T");//fixme
			if(y<=userscale*l.getY()+userscale*NodeHeight and y>=userscale*l.getY())
			{
				return l;
			}
		}
		return nil;
	},
	method possibleLevel(y,userscale)
	{
		spw::print("possibleLevel",nl);
		
//		return Levels.back();
	local ClosestLevel	= Levels.front();
	function distance(a,b)
	{
		return abs(b-a);
	}
	
	foreach(l,Levels)
	{
		if(distance(y,l.getY())<distance(y,ClosestLevel.getY()))
			ClosestLevel	= l;
	}
	return ClosestLevel;
/*		if(abs(y-Levels.front().getY())<abs(y-Levels.back().getY()))
			return Levels.front();
		else
			return Levels.back();
*/	},
	method DetectObject(x,y,userscale)
	{
		local l	= @detectLevel(x,y,userscale);
		if(l and (result = l.LookInLevel(userscale,x,y))!=nil)
			return result;
		return nil;
	},
	method apply(CurrList)
	{
		local ScriptsPerLevel		= spw::getproperty(::classId, "ScriptsPerLevel").value;
		Levels.push_back(newLevel(0));
		@print("=== Loading Graph ===");
		SpaceBetNodes	=	spw::getproperty(::classId, "SpaceBetNodes").value;
		//local CurrList	= Scripts;
		foreach(s,CurrList)
		{
			s.CurrDependencies	= s.dependencies;
			s.CurrNeighbours	= s.neighbours;
		}
		local level	= 0;
		foreach(p,Projects)
		{
			counter		= 1;
			tempLevel	= Levels.back();
			foreach(s,p.script_deps)
			{
				local node	= 
				s.node		= newNode(Nodes);
				node.GetPath= self.GetPath;
				node.script	= s;
				local name	= s.GetFName();
				local width	= @getWidth(name);
				local height= @getHeight(name);
				node.name	= name;
				node.width	= width;
				node.height	= height;
				node.setLevel(tempLevel);
				if(counter % ScriptsPerLevel == 0)
				{
					Levels.push_back(newLevel(Levels.total()));
					tempLevel	= Levels.back();
				}
				counter++;
			}
			if(tempLevel.Scripts.total()!=0)
				Levels.push_back(newLevel(Levels.total()));
		}
		local l = maxLevel(Levels);
		
		LevelWidth	= l.size + ((l.Scripts.total()-1)*SpaceBetNodes);
		
		@SetScripts(LevelWidth);
		@print("=== Connect ===");
		foreach(l,Levels)
		{
			spw::print("Sto LEvel :",l.num," bazw ta :",nl);
			foreach(s,l.Scripts){
				spw::print("\t",s.name,"(",s.x,",",s.y,")",nl);
			}
		}
//		foreach(l,Levels)
//		{
		foreach(s,Scripts)
		{
			foreach(d,s.CurrDependencies)
			{
				@ConnectScripts(s.node ,d.node);
			}
		}

//		}
	},
	method displayProjects()
	{
		spw::print("H lista me ta projects exei ta exhs:\n");
		foreach(p,Projects)
		{
			spw::print("\t",p.name,nl);
			foreach(d,p.dependencies)
			{
				spw::print("\t\t",d.name,nl);
			}
		}
		@apply(Projects);
	},
	method displayScripts()
	{
		@apply(Scripts);
	},
	method IncreaseSpace()
	{
		spw::print("Increase SPace\n");
		
		//spw::getproperty(::classId, "SpaceBetNodes").value	=	spw::getproperty(::classId, "SpaceBetNodes").value +100;
		local l = maxLevel(Levels);
		local DistanceStep	= spw::getproperty(::classId,"DistanceStep").value;
		SpaceBetNodes	+= DistanceStep;
		LevelWidth	= l.size+((l.Scripts.total()-1)*SpaceBetNodes);
		@SetScripts(LevelWidth);
		foreach(l,Levels)
		{
			foreach(node,l.Scripts)
			{
				node.reconnect();
				spw::print("reconnect:",node.name,nl);
			}
		}
		
	},
	method DecreaseSpace()
	{
		spw::print("Increase SPace\n");
		local l = maxLevel(Levels);
		local DistanceStep	= spw::getproperty(::classId,"DistanceStep").value;
		SpaceBetNodes	-= DistanceStep;
		LevelWidth	= l.size+((l.Scripts.total()-1)*SpaceBetNodes);
		@SetScripts(LevelWidth);
		foreach(l,Levels)
		{
			foreach(node,l.Scripts)
			{
				node.reconnect();
			}
		}
	}
	];
}

//------------------------------------------------

//~~~~~~~ INTERACT ~~~~~~~~~
function interact1()
{
	local selected	= nil;
	local lastX	= 0;
	local lastY	= 0;
	return [
		method onLeftDClickEvent(window,evt)
		{
			local obj = @DetectObject(evt.getx(),evt.gety(),window.userscale);
			selected = obj;
			if(obj)
			{
				spw::print("DCLICK on ",obj.name,nl);
				@apply(obj.file);
				selected	= nil;
				window.repaint();
			}
		},

		method onLeftDownEvent(window,evt)
		{
			spw::print("Left Down Event\n");
					
			spw::print("evt.getx() = ",evt.getx(),",evt.gety() = ",evt.gety(),nl);
			local dc	= window.dc;
			local logicalX	= dc.devicetologicalx(evt.getx());
			local logicalY	= dc.devicetologicaly(evt.gety());
			local obj = @DetectObject(logicalX, logicalY, window.userscale);
			if(obj!=selected and selected)
				selected.reset();
			selected = obj;
			if(selected)
				selected.highlight();
			else
				spw::print("Den anixneu8hke tpt!!\n");
			lastX	= logicalX;
			lastY	= logicalY;
			spw::print("Telos left down event!\n");
			window.repaint();
		},
		method onLeftUpEvent(window,evt)
		{
			local dc		= window.dc;
			local logicalX	= dc.devicetologicalx(evt.getx());
			local logicalY	= dc.devicetologicaly(evt.gety());
			spw::print("evt.getx()	= ",evt.getx(),"\t evt.gety()	= ",evt.gety(),nl);
			spw::print("logicalX	= ",logicalX,"\t logicalY	= ",logicalY,nl);

			if(selected)
			{
				selected.stopMoving(@possibleLevel,window.userscale);
			}
			window.repaint();
			
		},

		method onMotionEvent(window, evt) {
			
			if (evt.leftisdown()) {
				spw::print("x = ",evt.getx()," , y = ",evt.gety(),nl);
				
				local logicalX = window.dc.devicetologicalx(evt.getx());
				local logicalY = window.dc.devicetologicaly(evt.gety());		
				
				
				if(selected)
				{
					spw::print("kinw to ",selected.name,nl);
					selected.move(logicalX - lastX, logicalY - lastY);
					lastX	= logicalX;
					lastY	= logicalY;
					//selected.level	= @possibleLevel(selected.y,window.userscale);
					//selected.changeLevel(@possibleLevel(selected.y,window.userscale));
					//selected.y	= selected.level.getY();
					selected.reconnect();
					window.repaint();
				}

			}
		},
		method init(window)
		{
			window.connect("EVT_LEFT_DOWN",	 @onLeftDownEvent	);
			window.connect("EVT_LEFT_UP",	 @onLeftUpEvent		);
			window.connect("EVT_LEFT_DCLICK", @onLeftDClickEvent	);
			window.connect("EVT_MOTION", 	 @onMotionEvent		);
			
		}
	];
}
//------------------------------------------------


//~~~~~~~ RENDERER ~~~~~~~~~
function wxRenderer() {
	

	return [
//		@dc_type:	wx::buffereddc_construct, 
		@window	: 	0,
		@Lines	: 	0,
		@Nodes	: 	0,
		@shape	: 	0,
		@message: 	0,
		@frame	: 0,
		method getColour(id)
		{
			local obj = spw::getproperty(::classId, id).value;
			return colour_construct(obj.r, obj.g, obj.b);
		},
		method getFont(id)
		{
			local obj	= spw::getproperty(::classId, id).value;
			local size	= obj.size;
			local family= obj.family;
			local style	= obj.style;
			local weight= obj.weight;
			return font_construct( size, family, style, weight);
		},
		method Ellipse(dc,x,y,width,height)
		{
			wx::dc_drawellipse(dc,x,y,width,height);
		},
		method Rect(dc,x,y,width,height)
		{
			wx::dc_drawrectangle(dc,x,y,width,height);
		},
		method RoundRect(dc,x,y,width,height)
		{
			local arc	=spw::getproperty(::classId,"ShapeArc").value;
			wx::dc_drawroundedrectangle(dc,x,y,width,height,arc);
		},
		method getTextWidth	(text)
		{
			local dc	= bufferedpaintdc_construct(@window);
			dc.setfont(@getFont("Font"));
			spw::print("To ",text," pianei se width = ",dc.gettextextent(text).getwidth(),nl);
			return dc.gettextextent(text).getwidth();
		},
		method getTextHeight(text)
		{
			local dc	= bufferedpaintdc_construct(@window);
			spw::print("To ",text," pianei se height = ",dc.gettextextent(text).getheight(),nl);

			dc.setfont(@getFont("Font"));
			return dc.gettextextent(text).getheight();
		},				
		method displayNodes(dc)
		{
			local maxX	= 0;
			local maxY	= 0;
			counter =1;
			local shape_index	= spw::getproperty(::classId,"NodeShape").index;
			if(shape_index	== 0)
			{
				@shape	= @Ellipse;

			}
			else if(shape_index == 1)
			{
				@shape	= @Rect;

			}
			else if(shape_index==2)
			{
				@shape	=@RoundRect;
			}
			foreach(node,@Nodes)
			{
				local x			= node.x;
				local y			= node.y;
				local width		= node.width;
				local height	= node.height;	
				local name		= node.name;
				local BorderWidth		= node.BorderWidth;
				if(	x + width > maxX )
					maxX	= x + width;
				if( y + height > maxY )
					maxY	= y + height;
					
				local brush_colour	= @getColour(node.BGcolour);
				local brush			= brush_construct(brush_colour);
				
				local pen_colour	= @getColour(node.Shape_Colour);
				local pen			= pen_construct(pen_colour,BorderWidth);
				
				dc.setbrush(brush);
				dc.setpen(pen);
				@shape(dc,x,y,width,height);
				dc.drawtext(name,x,y);
			}
			@window.setvirtualsize(maxX, maxY);
			
		},
		method drawPointer(dc,p1,p2)
		{
			
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
			local Points= [point_construct(Cx,Cy),point_construct(D1x,D1y),point_construct(Bx,By),point_construct(D2x,D2y),point_construct(Cx,Cy)];
			dc.drawpolygon(5,Points);
		},

		method displayLines(dc)
		{
			counter	= 1;
			local temp_list	= list_new();
			foreach(local l ,@Lines)//fixme
			{
				local colour	= @getColour(l.colour);
				local width		= l.width;
				local pen		= pen_construct(colour,width);
				dc.setpen(pen);
				local Points 	= l.Points;
				dc.drawspline(tablength(Points),Points);
				@drawPointer(dc,Points[tablength(Points)-2],Points[tablength(Points)-1]);

			}

		},
		method displayMessage(dc,text)
		{
			local colour	= @getColour("BGColour");
			dc.setbackground(brush_construct(colour, "SOLID"));
			dc.clear();
			dc.setfont(@getFont("MessageFont"));
			
			local x			= 100;
			local y			= 100;
			local width		= 300;
			local height	= 200;
			local colour	= @getColour("MessageBGColour");
			local brush		= brush_construct(colour);
			dc.setbrush(brush);
			@RoundRect(dc,x,y,width,height);
			dc.drawtext(text,x,y);
		},
		method setMessage(text)
		{
			spw::print("SET MESSAGE\n");
			@message	= text;
			@window.repaint();

			spw::print("END OF Set Message \n");
			
		},
		method repaint()
		{
			local window	= @window;
			local dc	= clientdc_construct(window);
			local colour	= @getColour("BGColour");
			dc.setbackground(brush_construct(colour, "SOLID"));
			dc.clear();
			window.dc		= dc;
			dc.setuserscale(window.userscale, window.userscale);
			dc.setfont(@getFont("Font"));

			window.preparedc(dc);
			
			if (std::isnil(@message)) {
				dc.setuserscale(window.userscale, window.userscale);
				@displayLines(dc);
				@displayNodes(dc);
			
			} else {
				@displayMessage(dc,@message);
			}
			//window.dc	= nil;
			
		},
		method @operator()(Data)
		{
			@Nodes	= Data.Nodes;
			@Lines	= Data.Lines;
			@setMessage(nil);
		},
		method ViewProjects(frame,evt)
		{
			@displayProjects();
			frame.scrolledwindow.repaint();
		},
		method ViewScripts(frame,evt)
		{
			@displayScripts();
			frame.scrolledwindow.repaint();
		},
		method zoomout(frame,evt)
		{
			frame.scrolledwindow.userscale		/= 1.2;
			frame.scrolledwindow.repaint();
		},
		method zoomin(frame,evt)
		{
			frame.scrolledwindow.userscale		*= 1.2;
			frame.scrolledwindow.repaint();
		},
		method IncrSpace(frame,evt)
		{
			@IncreaseSpace();
			frame.scrolledwindow.repaint();
		},
		method DecrSpace(frame,evt)
		{
			@DecreaseSpace();
			frame.scrolledwindow.repaint();
		},
		method init()
		{
			frame = wx::frame_construct(nil, "ID_ANY", "Visualization Dependencies");
			@frame	= frame;
			frame.show(true);
			frame.scrolledwindow	= wx::scrolledwindow_construct(frame, wx::ID_ANY);
			@window	= frame.scrolledwindow;
			@window.repaint	= @repaint;
//			frame.scrolledwindow.setvirtualsize(5000, 5000);
			frame.scrolledwindow.setscrollrate(10, 10);
			
			//frame.scrolledwindow.connect(wx::EVT_PAINT, @repaint);
			
			frame.scrolledwindow.scrollX	= 0;
			frame.scrolledwindow.scrollY	= 0;
			frame.scrolledwindow.userscale	= 1;
			frame.scrolledwindow.dc			= clientdc_construct(frame.scrolledwindow);
			//frame.scrolledwindow.dc.setfont(font_construct("SWISS_FONT"));
			frame.setsize(815, 496);

			Menu = menu_construct();
			helpMenu = menu_construct();
			menus = [Menu, helpMenu];
			titles = ["&File", "&Help"];
		   
			menuBar = menubar_construct(2, menus, titles);
			frame.setmenubar(menuBar);
			menuBar.enabletop(1, true);

			Menu.append(4, "&ZoomIn\tCtrl-+", "Zoomin");
			Menu.append(5, "&ZoomOut\tCtrl--", "ZoomOut");
			Menu.appendseparator();
			Menu.append(6, "&ZoomOut\tCtrl-W", "View Workspace");
			Menu.append(7, "&ZoomOut\tCtrl-P", "View Projects");
			Menu.append(2, "E&xit\tAlt-F4", "Close the window");
			Menu.append(8, "E&xit\tAlt-+", "Increase Space");
			Menu.append(9, "E&xit\tAlt--", "Decrease Space");
			
			workspace_bmp 	= bitmap_construct("WorkSpace.bmp", "BITMAP_TYPE_BMP");
			projects_bmp 	= bitmap_construct("Projects.bmp", "BITMAP_TYPE_BMP");
			
			incSpace_bmp 	= bitmap_construct("IncreaseSpace.bmp", "BITMAP_TYPE_BMP");
			decSpace_bmp 	= bitmap_construct("DecreaseSpace.bmp", "BITMAP_TYPE_BMP");
			
			
			toolbar = frame.createtoolbar();
			toolbar.addtool(6, "WorkSpace", workspace_bmp, "View WorkSpace");
			toolbar.addtool(7, "Projects", projects_bmp, "View Projects");
			toolbar.addseparator();	
			
			toolbar.addtool(8, "IncreaseSpace", incSpace_bmp, "Increase Space between Nodes");
			toolbar.addtool(9, "DecreaseSpace", decSpace_bmp, "Decrease Space between Nodes");
			toolbar.addseparator();

			toolbar.realize();
			helpMenu.append(3,"&About\tAlt-O", "About developer");
			frame.connect(4, "EVT_COMMAND_MENU_SELECTED", @zoomin);
			frame.connect(5, "EVT_COMMAND_MENU_SELECTED", @zoomout);
			frame.connect(6, "EVT_COMMAND_MENU_SELECTED", @ViewScripts);
			frame.connect(7, "EVT_COMMAND_MENU_SELECTED", @ViewProjects);
			
			frame.connect(8, "EVT_COMMAND_MENU_SELECTED", @IncrSpace);
			frame.connect(9, "EVT_COMMAND_MENU_SELECTED", @DecrSpace);
			
			frame.connect(9, "EVT_MOVE", @repaint);
			//frame.scrolledwindow.connect(wx::EVT_SCROLLWIN_LINEDOWN, onScrollLineDown);
			//frame.scrolledwindow.connect(wx::EVT_SCROLLWIN_LINEUP, onScrollLineUp);
			@shape	= @RoundRect;
			
		}
	];
}
//~~~~ ~~~~ ~~~~ END OF RENDERER ~~~~ ~~~~ ~~~~ 


function SPVisual() {
	
	local renderer	= wxRenderer();
	renderer.init();
	
	renderer.setMessage( "== Collect Scripts ==" );
	local Data		= collectData();
	local Geometry	= g1(Data);
	
	
	local Scripts	= Data.Scripts;
	local Projects	= Data.Projects;
	local Lines		= Data.Lines;
	local Nodes		= Data.Nodes;
	Geometry.print		= renderer.setMessage;
	Geometry.getWidth	= renderer.getTextWidth;
	Geometry.getHeight	= renderer.getTextHeight;
	
	local window	= renderer.window;
	local inter		= interact1();
	inter.DetectObject	= Geometry.DetectObject;
	inter.possibleLevel	= Geometry.possibleLevel;

	inter.init(window);
	
	spw::print("*** SCRIPTS ***\n");
	foreach(s,Scripts)
		spw::print("\t",s.name,nl);
	spw::print("*** PROJECTS ***\n");
	foreach(p,Projects)
		spw::print("\t",p.name,nl);

	Geometry.apply(Data.Scripts);
	Geometry.warning	= spw::print;
	inter.apply	= Geometry.apply;
	renderer.displayProjects	= Geometry.displayProjects;
	renderer.displayScripts		= Geometry.displayScripts;
	renderer.IncreaseSpace		= Geometry.IncreaseSpace;
	renderer.DecreaseSpace		= Geometry.DecreaseSpace;
	renderer(Data);
}



//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
    spw::registercomponent(classId);
    spw::setcomponentmetadata(
        classId, "Dependencies between Projects and Scripts",
        "A utility for visualization dependencies in opened workspace",
        "Koutsouroumpis Georgios <gkutsur@csd.uoc.gr>",
        "alpha"
    );

    spw::registerimage("linecount", spw::installationdir() + "/resources/linecount.png");

    spw::class_decl_required_member_function(classId, "CountLines", "uint (void)",
        "Return the total line of codes in the workspace");
   spw::class_decl_required_member_command(
        [
            {.class            : "UserCommandDesc"    },
            {.class_id        : classId            },
            {.function_name    : "CountLinesCmd"    },
            {.flags            : 7                    },
            {.image            : "linecount"        }
        ],
        "/{200}Tools/{100}LineCounter",
        "Display the dependencies between projects and scripts"
    );

spw::class_impl_static_command(
		[
			{.class			: "UserCommandDesc"				},
			{.class_id		: classId						},
			{.function_name	: "ConfigureBuildTreeListView"	},
			{.flags			: 1								}
		],
		"/{2}Configure/LineCounter...",
		(function Configure {spw.components.Shell.ConfigureComponent(classId); }),
		"Configure the LineCounter"
	);
	
	SetProperties();
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
    spw::inst_impl_required_member_function(classId, "CountLines", SPVisual);
    spw::inst_impl_required_member_command(classId, "CountLinesCmd", SPVisual);
	
	
}

//-----------------------------------------------------------------------

onevent Destructor
{
}

//-----------------------------------------------------------------------