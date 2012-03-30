

function strisnotspace (str) {
	for(i=0;i<std::strlen(str);++i)
		if(not std::strisspace(std::strbyte(str,i)))
			return true;
	return false;
}


function devicetological(scrolledwindow, x, y, scale)
{
    deviceOrigin = scrolledwindow.calcscrolledposition(0, 0);
    local logicalX = std::integerpart((x - deviceOrigin.getx()) / scale);
    local logicalY = std::integerpart((y - deviceOrigin.gety()) / scale);
    return wx::point_construct(logicalX, logicalY);
}

function logicaltodevice(scrolledwindow, x, y, scale)
{
    deviceOrigin = scrolledwindow.calcscrolledposition(0, 0);
    local deviceX = std::integerpart(x * scale) + deviceOrigin.getx();
    local deviceY = std::integerpart(y * scale) + deviceOrigin.gety();
    return wx::point_construct(deviceX, deviceY);
}
