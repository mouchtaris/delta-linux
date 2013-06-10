using #ImageHolder;

const nl = "\n";


/****        Super Class Of Shapes        ****/

function Shape (x, y, width, height) {
	return [
		const r = 6,
		@class			: #Shape,
		@x				: x,
		@y				: y,
		@width			: width,
		@height			: height,
		@rect			: wx::rect_construct(x, y, width, height),
		@centralPoint	: wx::point_construct(x+width/2, y+height/2),
		method getCentralPoint				{ return self.centralPoint;		 },
		method setCentralPoint (ptcentral)	{ self.centralPoint = ptcentral; },
		method setPoint (x,y)               {
			self.x = x;
			self.y = y;
			self.rect.setx(x);
			self.rect.sety(y);
			self.centralPoint = wx::point_construct(x+self.width/2, y+self.height/2);
		},
		method setValues (x,y,width,height) {
			self.x = x;
			self.y = y;
			self.width = width;
			self.height = height;
			self.rect = wx::rect_construct(x,y,width,height);
			self.centralPoint = wx::point_construct(x+width/2, y+height/2);
		},
		method pointLine (pt)				{ assert(false); },
		method drawShape (dc)				{ assert(false); },
		method drawEnable (dc, isOrphan)	{ assert(false); },
		method drawElement (dc,str...)		{ assert(false); },
		method drawShapeMotion (dc)			{ assert(false); },
		method drawElementEnable (dc)		{ assert(false); },
		method drawHighlight (dc)			{ assert(false); },
		method drawSelected	(dc) {
			bitmap = wx::bitmap_construct(self.width+10, self.height+10);
			bdc = wx::memorydc_construct();
			bdc.selectobject(bitmap);
			local brush = wx::brush_construct(colour = wx::colour_construct(255,255,255));
			local pen   = wx::pen_construct(colour);
			bdc.setbrush(brush);
			bdc.setpen(pen);
			bdc.drawrectangle(0,0, bitmap.getwidth(),bitmap.getheight());
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			bdc.setpen(pen);
			for(i=0;i<self.width*2;i=i+4)
				bdc.drawline(0,i,i,0);
			//draw magenda rectangle
			local pen = wx::pen_construct(colour = wx::colour_construct("#FF00FF"));
			bdc.setpen(pen);
			local brush = wx::brush_construct(colour);
			bdc.setbrush(brush);
			bdc.drawrectangle(5, 5, self.width, self.height);
			bdc.selectobject(wx::bitmap_construct());
			// tranparrent rectangle
			mask = wx::mask_construct(bitmap, colour);
			bitmap.setmask(mask);
			dc.drawbitmap(bitmap, wx::point_construct(@x-5,@y-5),true);
			return;
		},
		method drawRects (dc) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			dc.setpen(pen);
			local brush = wx::brush_construct(colour);
			dc.setbrush(brush);
			dc.drawrectangle(self.x, self.y, r, r);
			dc.drawrectangle(self.x+self.width/2-r/2, self.y, r, r);
			dc.drawrectangle(self.x+self.width-r, self.y, r, r);
			dc.drawrectangle(self.x, self.y+self.height-r, r, r);
			dc.drawrectangle(self.x+self.width/2-r/2, self.y+self.height-r, r, r);
			dc.drawrectangle(self.x+self.width-r, self.y+self.height-r, r, r);
			dc.drawrectangle(self.x, self.y+self.height/2-r/2, r, r);
			dc.drawrectangle(self.x+self.width-r, self.y+self.height/2-r/2, r, r);
		},
		method contains (pt) {
			return @rect.contains(pt);
		},
		method intersect (rect) {
			return @rect.intersect(rect);
		},
		method onRects (pt) {
			rect = wx::rect_construct(self.x, self.y, r, r);
			if (rect.contains(pt))
				return #leftup;
			rect = wx::rect_construct(self.x+self.width/2-r/2, self.y, r, r);
			if (rect.contains(pt))
				return #up;
			rect = wx::rect_construct(self.x+self.width-r, self.y, r, r);
			if (rect.contains(pt))
				return #rightup;
			rect = wx::rect_construct(self.x, self.y+self.height-r, r, r);
			if (rect.contains(pt))
				return #leftdown;
			rect = wx::rect_construct(self.x+self.width/2-r/2, self.y+self.height-r, r, r);
			if (rect.contains(pt))
				return #down;
			rect = wx::rect_construct(self.x+self.width-r, self.y+self.height-r, r, r);
			if (rect.contains(pt))
				return #rightdown;
			rect = wx::rect_construct(self.x, self.y+self.height/2-r/2, r, r);
			if (rect.contains(pt))
				return #left;
			rect = wx::rect_construct(self.x+self.width-r, self.y+self.height/2-r/2, r, r);
			if (rect.contains(pt))
				return #right;
			return false;
		},
		method savexml (obj) { assert(false); }
	];
}


/****        Shape: Rectangle                      ****/

function Rectangle(x, y, width, height) {
	return [
		@class	: #Rectangle,
		std::inherit( @self, ::Shape(x,y,width,height) ),
		method pointLine (pt) {
			assert(false);
			// the linesegment and the
		},
		method drawShape (dc, selected) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			dc.setpen(pen);
			local brush = wx::brush_construct(colour = wx::colour_construct("#FFFF99"));
			dc.setbrush(brush);
			dc.drawrectangle(self.x,self.y,self.width,self.height);
			if (selected)
				self.drawRects(dc);
		},
		method drawEnable (dc) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0), 4, wx::DOT);
			dc.setpen(pen);
			local brush = wx::brush_construct(colour, wx::TRANSPARENT);
			dc.setbrush(brush);
			dc.drawrectangle(self.x,self.y,self.width,self.height);
		},
		method drawText (dc, shortDesc) {
//			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			dc.drawtext(shortDesc, self.x+5, self.y+20);
		},
		method drawShapeMotion (dc) {
			assert(false);
		},
		method drawElementEnable (dc) {
			assert(false);
		},
		method drawHighlight (dc) {
			theta = 0;
			colourNum = 220 * std::cos(theta);
			for (i=6;i>=0;--i) {
				local brush = wx::brush_construct(colour = wx::colour_construct(255,colourNum,colourNum));
				dc.setbrush(brush);
				local pen = wx::pen_construct(colour);
				dc.setpen(pen);
				dc.drawrectangle(self.x-i,self.y-i,self.width+(i*2),self.height+(i*2));
				theta +=18;
				colourNum = 220*std::cos(theta);
			}
		},
		method savexml (obj) {
			obj.x = self.x;
			obj.y = self.y;
			obj.width = self.width;
			obj.height = self.height;
			obj.centralPoint = [];
			obj.centralPoint[0] = [@x:self.centralPoint.getx(),@y:self.centralPoint.gety()];
		}
	];
}

/************************************/
/* shape:	Rectangular				*/
/************************************/

function Rectangular (x, y, width, height) {
	const dist = 10;
	return [
		@class	: #Rectangular,
		std::inherit(@self, ::Shape(x, y, width, height)),
		@point1	: wx::point_construct (x+dist,       y       ), // up & left	point of rectangular 
		@point2	: wx::point_construct (x,			 y+height), // down & left	point of rectangular
		@point3	: wx::point_construct (x-dist+width, y+height), // down & right	point of rectangular
		@point4	: wx::point_construct (x+width,      y       ), // up & right	point of rectangular
		@distBB	: dist,
		
		method setDistanceBB (distance) { @distBB = distance; },
		
		method pointLine (pt) {
			assert(false);
		},
		method setPoint (x,y) {
			std::getbase(self,0)..setPoint(x,y);
			@point1	= wx::point_construct(x+@distBB, y);
			@point2	= wx::point_construct(x, y+self.height);
			@point3	= wx::point_construct(x-@distBB+self.width, y+self.height);
			@point4	= wx::point_construct(x+self.width, y);
		},
		method setValues (x,y,width,height) {
			std::getbase(self,0)..setValues(x,y,width,height);
			@point1	= wx::point_construct(x+@distBB, y);
			@point2	= wx::point_construct(x, y+height);
			@point3	= wx::point_construct(x-@distBB+width, y+height);
			@point4	= wx::point_construct(x+width, y);
		},
		method drawShape (dc, selected) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			dc.setpen(pen);
			local brush = wx::brush_construct(colour = wx::colour_construct("#FFFF99"));
			dc.setbrush(brush);
			dc.drawpolygon(4,[self.point1,self.point2,self.point3,self.point4]);
			if (selected)
				self.drawRects(dc);
		},
		method drawEnable (dc) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0), 3, wx::DOT);
			dc.setpen(pen);
			local brush = wx::brush_construct(colour, wx::TRANSPARENT);
			dc.setbrush(brush);
			dc.drawpolygon(4,[self.point1,self.point2,self.point3,self.point4]);
		},
		method drawText (dc, shortDesc) {
			dc.drawtext(shortDesc, self.x+10, self.y+20);
		},
		method drawShapeMotion (dc) {
			assert(false);
		},
		method drawElementEnable (dc) {
			assert(false);
		},
		method drawHighlight (dc) {
			theta = 0;
			colourNum = 220 * std::cos(theta);
			for (i=5;i>=0;--i) {
				local brush = wx::brush_construct(colour = wx::colour_construct(255,colourNum,colourNum));
				dc.setbrush(brush);
				local pen = wx::pen_construct(colour);
				dc.setpen(pen);
				pt1	= wx::point_construct(self.x+@distBB-i, self.y-i);
				pt2	= wx::point_construct(self.x-i, self.y+self.height+i);
				pt3	= wx::point_construct(self.x-@distBB+self.width+i, self.y+self.height+i);
				pt4	= wx::point_construct(self.x+self.width+i, self.y-i);
				dc.drawpolygon(4, [pt1,pt2,pt3,pt4]);
				theta +=22;
				colourNum = 220*std::cos(theta);
			}
		},
		method savexml (obj) {
			obj.x = self.x;
			obj.y = self.y;
			obj.width = self.width;
			obj.height = self.height;
			obj.centralPoint = [];
			obj.centralPoint[0] = [{#x:self.centralPoint.getx()},{#y:self.centralPoint.gety()}];
			obj.point = [];
			obj.point[0] = [{#x:self.point1.getx()},{#y:self.point1.gety()}];
			obj.point[1] = [{#x:self.point2.getx()},{#y:self.point2.gety()}];
			obj.point[2] = [{#x:self.point3.getx()},{#y:self.point3.gety()}];
			obj.point[3] = [{#x:self.point4.getx()},{#y:self.point4.gety()}];
		}
	];
}


/************************************/
/*		Circle	 					*/
/************************************/

function Circle (x, y, radius) {
	return [
		@class	: #Circle,
		std::inherit(@self, ::Shape(x-radius, y+radious, 2*radius, 2*radius)),
		@radius : radius,
		//perfect collision
		method CollisionCheck (pt) {
			assert( pt and pt.getx() and pt.gety() );
			// (X-Xo)^2+(Y-Yo)^2 <= radius^2
			return ( std::power(self.x-pt.getx(), 2) + std::power(self.y-pt.gety(), 2) <= std::power(@radius, 2) );
		},
		method pointLine (pt) {},
		method drawShape (dc, selected) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			dc.setpen(pen);
			local brush = wx::brush_construct(colour = wx::colour_construct("#FFFF99"));
			dc.setbrush(brush);
			dc.drawcircle(x,y,radius);
			if (selected)
				self.drawRects(dc);
		},
		method drawText (dc, shortDesc) {
			dc.drawtext(shortDesc, self.x+5, self.y+20);
		},
		method drawShapeMotion (dc) {
			assert(false);
		},
		method drawElementEnable (dc) {
			assert(false);
		},
		method drawHighlight(dc) {
			theta = 0;
			colourNum = 220 * std::cos(theta);
			for (i=5;i>=0;--i) {
				local pen = wx::pen_construct(colour = wx::colour_construct(255,colourNum,colourNum));
				dc.setpen(pen);
				local brush = wx::brush_construct(colour);
				dc.setbrush(brush);
				dc.drawcircle(self.x, self.y, @radius+i);
				theta +=22;
				colourNum = 220*std::cos(theta);
			}
		},
		method savexml (obj) {
			obj.x = self.x;
			obj.y = self.y;
			obj.width = self.width;
			obj.height = self.height;
			obj.centralPoint = [];
			obj.centralPoint[0] = [{#x:self.centralPoint.getx()},{#y:self.centralPoint.gety()}];
			obj.radius = @radius;
		}
	];
}


/************************************/
/*		Ellipse	 					*/
/************************************/

function Ellipse(x,y,width,height) {
	return [
		@class	: #Ellipse,
		std::inherit( @self, ::Shape(x, y, width, height) ),
		method pointLine (pt) {},
		method drawShape (dc, selected) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			dc.setpen(pen);
			local brush = wx::brush_construct(colour = wx::colour_construct("#FFFF99"));
			dc.setbrush(brush);
			dc.drawellipse(self.rect);
			if (selected)
				self.drawRects(dc);
		},
		method drawEnable (dc) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0), 3, wx::DOT);
			dc.setpen(pen);
			local brush = wx::brush_construct(colour, wx::TRANSPARENT);
			dc.setbrush(brush);
			trect = wx::rect_construct(self.rect.getx(),self.rect.gety(),self.rect.getwidth(),self.rect.getheight());
			dc.drawellipse(trect);
		},
		method drawText (dc, shortDesc) {
			dc.drawtext(shortDesc, self.x+20, self.y+30);
		},
		method drawShapeMotion (dc) {
			assert(false);
		},
		method drawElementEnable (dc) {
			assert(false);
		},
		method drawHighlight (dc) {
			theta = 0;
			colourNum = 220 * std::cos(theta);
			for (i=5;i>=0;--i) {
				local pen = wx::pen_construct(colour = wx::colour_construct(255,colourNum,colourNum));
				dc.setpen(pen);
				local brush = wx::brush_construct(colour);
				dc.setbrush(brush);
				dc.drawellipse(wx::rect_construct(self.x-i, self.y-i, self.width+(i*2), self.height+(i*2)));
				theta +=22;
				colourNum = 220*std::cos(theta);
			}
		},
		method savexml (obj) {
			obj.x = self.x;
			obj.y = self.y;
			obj.width = self.width;
			obj.height = self.height;
			obj.centralPoint = [];
			obj.centralPoint[0] = [{#x:self.centralPoint.getx()},{#y:self.centralPoint.gety()}];
		}
	];
}


/************************************/
/*		Rhombus	 					*/
/************************************/

function Rhombus(x, y, width, height) {
	return [
		@class	: #Rhombus,
		std::inherit(@self, ::Shape(x, y, width, height) ),
		@point1	: wx::point_construct( x+width/2, y ),
		@point2	: wx::point_construct( x, y+height/2 ),
		@point3	: wx::point_construct( x+width/2, y+height ),
		@point4	: wx::point_construct( x+width, y+height/2 ),
		//
		method setPoint (x,y) {
			std::getbase(self,0)..setPoint(x,y);
			@point1	= wx::point_construct( self.x+self.width/2, self.y );
			@point2	= wx::point_construct( self.x, self.y+self.height/2 );
			@point3	= wx::point_construct( self.x+self.width/2, self.y+self.height );
			@point4	= wx::point_construct( self.x+self.width, self.y+self.height/2 );
		},
		method setValues (x,y,width,height) {
			std::getbase(self,0)..setValues(x,y,width,height);
			@point1	= wx::point_construct( x+width/2, y );
			@point2	= wx::point_construct( x, y+self.height/2 );
			@point3	= wx::point_construct( x+width/2, y+height );
			@point4	= wx::point_construct( x+width, y+height/2 );
		},
		method drawShape (dc, selected) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0));
			dc.setpen(pen);
			local brush = wx::brush_construct(colour = wx::colour_construct("#FFFF99"));
			dc.setbrush(brush);
			dc.drawpolygon(4,[self.point1, self.point2, self.point3, self.point4]);
			if (selected)
				self.drawRects(dc);
		},
		method drawText (dc, shortDesc) {
			dc.drawtext(shortDesc, self.x+20, self.y+20);
		},
		method drawEnable (dc) {
			local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0), 3, wx::DOT);
			dc.setpen(pen);
			local brush = wx::brush_construct(colour, wx::TRANSPARENT);
			dc.setbrush(brush);
			dc.drawpolygon(4,[self.point1, self.point2, self.point3, self.point4]);
		},
		method drawHighlight (dc) {
			theta = 0;
			colourNum = 220 * std::cos(theta);
			for (i=5;i>=0;--i) {
				local pen = wx::pen_construct(colour =wx::colour_construct(255,colourNum,colourNum));
				dc.setpen(pen);
				local brush = wx::brush_construct(colour);
				dc.setbrush(brush);
				pt1 = wx::point_construct(self.x+self.width/2, self.y-i*1.6);
				pt2 = wx::point_construct(self.x-i*3, self.y+self.height/2);
				pt3 = wx::point_construct(self.x+self.width/2, self.y+self.height+i*1.6);
				pt4 = wx::point_construct(self.x+self.width+i*3, self.y+self.height/2);
				dc.drawpolygon(4,[pt1, pt2, pt3, pt4]);
				theta +=22;
				colourNum = 220*std::cos(theta);
			}
		},
		method savexml (obj) {
			obj.x = self.x;
			obj.y = self.y;
			obj.width = self.width;
			obj.height = self.height;
			obj.centralPoint = [];
			obj.centralPoint[0] = [{#x:self.centralPoint.getx()},{#y:self.centralPoint.gety()}];
			obj.point = [];
			obj.point[0] = [{#x:self.point1.getx()},{#y:self.point1.gety()}];
			obj.point[1] = [{#x:self.point2.getx()},{#y:self.point2.gety()}];
			obj.point[2] = [{#x:self.point3.getx()},{#y:self.point3.gety()}];
			obj.point[3] = [{#x:self.point4.getx()},{#y:self.point4.gety()}];
		}
	];
}

/*******************************************
	Line Segment UI
*******************************************/

/*
function PointLineSegments (pt1,pt2, pt3,pt4) {
	x1 = pt1.getx();
	x2 = pt2.getx();
	x3 = pt3.getx();
	x4 = pt4.getx();
	y1 = pt1.gety();
	y2 = pt2.gety();
	y3 = pt3.gety();
	y4 = pt4.gety();
	a1;a2;b1;b2;c1;c2;
	if (y1-y2==0) {
		a1 = 0;
		b1 = 1;
		c1 = (-1)*y1;
	} else
	if (x1-x2==0) {
		a1 = 1;
		b1 = 0;
		c1 = (-1)*x1;
	} else {
		a1 = (-1)*( (y2-y1)/(x2-x1) );
		b1 = 1;
		c1 = (-1)*( y1+ @a*x1 );
	}
	if (y3-y4==0) {
		a2 = 0;
		b2 = 1;
		c2 = (-1)*y3;
	} else
	if (x3-x4==0) {
		a2 = 1;
		b2 = 0;
		c2 = (-1)*x3;
	} else {
		a2 = (-1)*( (y4-y3)/(x4-x3) );
		b2 = 1;
		c2 = (-1)*( y3+ a2*x3 );
	}
	//
	if (a1==0 and a2==0) {
		return nil;
	}
	else
	if (a1==0) {
		x = x1;
		
	}
	else
	if (b1==0 and b2==0) {
		
	}
	else {
		numeratorY   = (a1*c2)/a2 -c1;
		denominatorY = (a1*b2)/a2 -b1;
		// there is no union in two line segments
		if (denominatorY==0)
			return nil;
		y = numeratorY / denominatorY;
		
	}
}

function LineSegment (point1, point2) {
	// a*X+b*Y+c=0
	tmp = [
		@point1	: point1,
		@point2	: point2,
		@a		: 0,
		@b		: 0,
		@c		: 0,
		method setLineSegment (pt1, pt2) {
			@point1 = pt1;
			@point2 = pt2;
			y1	= pt1.gety();
			y2	= pt2.gety();
			x1	= pt1.getx();
			x2	= pt2.getx();
			// pt1 != pt2
			assert( not( x1==x2 and y1==y2 ) );
			// equation a*X+b*Y+c=0 lookfor a, b, c
			if (y1-y2==0) {
				@a = 0;
				@b = 1;
				@c = (-1)*y1;
			}
			else
			if (x1-x2==0) {
				@a = 1;
				@b = 0;
				@c = (-1)*x1;
			}
			else {
				@a = (-1)*( (y2-y1)/(x2-x1) );
				@b = 1;
				@c = (-1)*( y1+ @a*x1 );
			}
		},
		method contains (pt) {
			// in the same straight
			inSameStraight = ( @a * pt.getx() + @b * pt.gety() + @c == 0 );
			if(not inSameStraight) {
				return false;
			}
			else {
				if (@a==1 and @b==0) {
					if (@point1.gety() > @point2.gety())
						return ( pt.gety()<=@point1.gety() and pt.gety()>=@point2.gety() );
					else
						return ( pt.gety()>=@point1.gety() and pt.gety()<=@point2.gety() );
				}
				else {
					if (@point1.getx() > @point2.getx())
						return ( pt.getx()<=@point1.getx() and pt.getx()>=@point2.getx() );
					else
						return ( pt.getx()>=@point1.getx() and pt.getx()<=@point2.getx() );
				}
			}
		}
	];
	tmp.setLineSegment(point1,point2);
	return tmp;
}
*/

/**
 *  Description: pt is the midpoint and dx,dy is the direction
 *
 **/
function DrawArrow(dc, pt, dx, dy) {
    cos = 0.866;
    sin = 0.500;
	end1 = wx::point_construct(pt.getx()+(dx*cos+dy*(-sin)), pt.gety()+(dx*sin+dy*cos));
	end2 = wx::point_construct(pt.getx()+(dx*cos+dy*sin), pt.gety()+(dx*(-sin)+dy*cos));
	dc.drawline(pt.getx(), pt.gety(), end1.getx(), end1.gety());
	dc.drawline(pt.getx(), pt.gety(), end2.getx(), end2.gety());
}


function Segment (lineWidth, lineStyle, lineColour, type, pt1,pt2,direction) {
	points;
	if (std::abs(pt1.getx()-pt2.getx()) >= std::abs(pt1.gety()-pt2.gety()) ) {
		points = [	wx::point_construct(pt1.getx(), pt1.gety()-2),
					wx::point_construct(pt1.getx(), pt1.gety()+2),
					wx::point_construct(pt2.getx(), pt2.gety()+2),
					wx::point_construct(pt2.getx(), pt2.gety()-2)	];
	} else {
		points = [	wx::point_construct(pt1.getx()-2, pt1.gety()),
					wx::point_construct(pt1.getx()+2, pt1.gety()),
					wx::point_construct(pt2.getx()+2, pt2.gety()),
					wx::point_construct(pt2.getx()-2, pt2.gety())	];
	}
	return [
		@line		: [@width : lineWidth, @style : lineStyle, @colour : lineColour],
		@point1		: pt1,
		@point2		: pt2,
		@direction	: direction,
		@image  	: ImageHolder::images()[type],
		@region		: wx::region_construct(4, points),
		method drawSegment (dc) {
			pen = wx::pen_construct(colour = wx::colour_construct(@line.colour), @line.width);
			dc.setpen(pen);
			dc.drawline(@point1,@point2);
			//
			local pen = wx::pen_construct(colour = wx::colour_construct(@line.colour), @line.width, @line.style);
			dc.setpen(pen);
			local x = (@point1.getx()+@point2.getx())/2;
			local y = (@point1.gety()+@point2.gety())/2;
			dc.drawbitmap(@image,x-8,y-8,true);
			midpoint = wx::point_construct(x,y);
			if (@direction==1) {
				dx = (@point2.getx() - @point1.getx());
				dx = dx/30;
				dy = (@point2.gety() - @point1.gety());
				dy = dy/30;
				::DrawArrow(dc, midpoint, dx, dy);
			}
			else {
				dx = (@point1.getx() - @point2.getx());
				dx = dx/30;
				dy = (@point1.gety() - @point2.gety());
				dy = dy/30;
				::DrawArrow(dc, midpoint, dx, dy);
			}
		},
		method setSegment (pt, ptnum) {
			if (ptnum==1)
				@point1 = pt;
			else
				@point2 = pt;
			points;
			if (std::abs(@point1.getx()-@point2.getx()) >= std::abs(@point1.gety()-@point2.gety()) ) {
				points = [	wx::point_construct(@point1.getx(), @point1.gety()-2),
							wx::point_construct(@point1.getx(), @point1.gety()+2),
							wx::point_construct(@point2.getx(), @point2.gety()+2),
							wx::point_construct(@point2.getx(), @point2.gety()-2)	];
			} else {
				points = [	wx::point_construct(@point1.getx()-2, @point1.gety()),
							wx::point_construct(@point1.getx()+2, @point1.gety()),
							wx::point_construct(@point2.getx()+2, @point2.gety()),
							wx::point_construct(@point2.getx()-2, @point2.gety())	];
			}
			@region = wx::region_construct(4, points);
		},
		
		method setValues (size,style,colour,x1,y1,x2,y2) {
			@line =	[@width : size, @style : style, @colour : colour];
			@point1	= wx::point_construct(x1,y1);
			@point2	= wx::point_construct(x2,y2);
			points;
			if (std::abs(@point1.getx()-@point2.getx()) >= std::abs(@point1.gety()-@point2.gety()) ) {
				points = [	wx::point_construct(@point1.getx(), @point1.gety()-2),
							wx::point_construct(@point1.getx(), @point1.gety()+2),
							wx::point_construct(@point2.getx(), @point2.gety()+2),
							wx::point_construct(@point2.getx(), @point2.gety()-2)	];
			} else {
				points = [	wx::point_construct(@point1.getx()-2, @point1.gety()),
							wx::point_construct(@point1.getx()+2, @point1.gety()),
							wx::point_construct(@point2.getx()+2, @point2.gety()),
							wx::point_construct(@point2.getx()-2, @point2.gety())	];
			}
			@region = wx::region_construct(4, points);
		},
		method savexml (obj) {
			obj.line = [];
			obj.line[0] = [];
			obj.line[0].width  = @line.width;
			obj.line[0].style  = @line.style;
			obj.line[0].colour = @line.colour;
			obj.point = [];
			obj.point[0] = [{#x:@point1.getx()},{#y:@point1.gety()}];
			obj.point[1] = [{#x:@point2.getx()},{#y:@point2.gety()}];
		}
	];
}

