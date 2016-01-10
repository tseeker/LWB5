// Generic text input component
TextInput = function (id, title, multi, min, max, init) {

	this.id = id;
	this.title = title;
	this.multi = multi;
	var value = init;
	this.min = min;
	this.max = max;
	this.valid = false;

	this.onChange = null;

	var readValue = function () {
		var e = document.getElementById(id + '-input');
		if (e) {
			value = e.value;
		}
	};

	this.getText = function () {
		readValue();
		return value;
	};

	this.draw = function () {
		readValue();

		var str = '<table style="width: 100%; border-style: none; padding: 2px; margin: 0px">'
			+ '<tr><th style="text-align:right; padding: 0px 5px; width: 120px;vertical-align:top">'
			+ this.title + ':</th><td style="text-align: center">';
		var jsStr = '="TextInput.byId[\'' + this.id + '\'].__change(); return true"';

		if (this.multi) {
			str += '<textarea id="' + this.id + '-input" rows="5" style="width: 100%" onChange'
				+ jsStr + ' onKeyUp' + jsStr + ' onClick' + jsStr + '></textarea>';
		} else {
			str += '<input type="text" id="' + this.id + '-input" style="width: 100%" onChange'
				+ jsStr + ' onKeyUp' + jsStr + ' onClick' + jsStr + '/>';
		}

		str += '</td></tr></table>';
		document.getElementById(this.id).innerHTML = str;
		document.getElementById(this.id + '-input').value = value;
		this.checkValidity();
	};

	this.checkValidity = function () {
		this.valid = (this.min == 0 && this.max == 0);
		if (!this.valid) {
			this.valid = (this.min > 0 && value.length >= this.min || this.min == 0)
				&& (this.max > 0 && value.length <= this.max || this.max == 0);
		}

		with (document.getElementById(this.id + '-input').style) {
			color = this.valid ? 'black' : 'white';
			backgroundColor = this.valid ? 'white' : 'red';
		}
	};

	this.__change = function () {
		readValue();
		this.checkValidity();
		if (this.onChange) {
			this.onChange(value);
		}
	};

	TextInput.byId[this.id] = this;
};
TextInput.byId = {};

// Generic numeric control for size and amount of alliances
NumericControl = function (id, title, min, max, init) {
	this.id = id;
	this.title = title;
	this.min = min;
	this.max = max;
	this.value = init;

	this.onChange = null;

	this.draw = function () {
		var str = '<table style="width: 100%; border-style: none; padding: 2px; margin: 0px">'
			+ '<tr><th style="text-align:right; padding: 0px 5px">' + this.title
			+ ':</th><td style="text-align: center; width: 20px">' + this.value
			+ '</td><td style="width: 70px; text-align:center">'
			+ '<input type="button" onClick="NumericControl.byId[\''
			+ this.id + '\'].decrease()" value="-" style="width: 24px" /> '
			+ '<input type="button" onClick="NumericControl.byId[\''
			+ this.id + '\'].increase()" value="+" style="width: 24px" /></td>'
			+ '</tr></table>';
		document.getElementById(this.id).innerHTML = str;
	};

	this.increase = function () {
		if (this.value == this.max) {
			return;
		}

		this.value ++;
		this.draw();
		if (this.onChange) {
			this.onChange(this.value);
		}
	};
	this.decrease = function () {
		if (this.value == this.min) {
			return;
		}

		this.value --;
		this.draw();
		if (this.onChange) {
			this.onChange(this.value);
		}
	};

	NumericControl.byId[this.id] = this;
};
NumericControl.byId = {};


// A system on the map
MapLocation = function (type, alloc, spawn) {
	this.type = type;
	this.alloc = alloc;
	this.spawn = spawn;
};


// The map itself
Map = function (initFrom) {
	// Copy basic map information
	this.name = initFrom.name;
	this.description = initFrom.description;
	this.alliances = initFrom.alliances;
	this.width = initFrom.width;
	this.height = initFrom.height;

	// Copy the map
	this.map = { };
	for (var i in initFrom.map) {
		var ma = initFrom.map[i];
		this.map['x' + ma[0] + 'y' + ma[1]] = new MapLocation(ma[2], ma[3], ma[4]);
	}

	// Function that computes min/max x/y
	this.updateCoordinates = function () {
		this.minX = - Math.floor(this.width / 2);
		this.maxX = this.minX + this.width - 1;

		this.minY = - Math.floor(this.height / 2);
		this.maxY = this.minY + this.height - 1;
	};

	// Creates a nebula area
	this.setNebula = function (x, y, opacity) {
		if (this.map['x' + x + 'y' + y]) {
			this.map['x' + x + 'y' + y].type = opacity;
		} else {
			this.map['x' + x + 'y' + y] = new MapLocation(opacity, null, null);
		}
	};

	// Creates a target system
	this.setTarget = function (x, y) {
		if (this.map['x' + x + 'y' + y]) {
			this.map['x' + x + 'y' + y].type = 'S';
			this.map['x' + x + 'y' + y].alloc = 0;
		} else {
			this.map['x' + x + 'y' + y] = new MapLocation('S', 0, null);
		}
	};

	// Creates an alliance-controlled system
	this.setAlliance = function (x, y, alliance) {
		var sys = this.map['x' + x + 'y' + y];
		if (sys) {
			sys.type = 'S';
			sys.alloc = alliance;
			sys.spawn = false;
		} else {
			this.map['x' + x + 'y' + y] = new MapLocation('S', alliance, false);
		}
	};

	// Switches spawning status for an alliance-controlled system
	this.switchSpawnPoint = function (x, y) {
		var sys = this.map['x' + x + 'y' + y];
		if (sys && sys.type == 'S' && sys.alloc > 0) {
			sys.spawn = !sys.spawn;
		}
	};

	// Removes systems allocated to alliances which no longer exist
	this.removeExtraAlliances = function () {
		for (var i in this.map) {
			var sys = this.map[i];
			if (sys.type == 'S' && sys.alloc > this.alliances) {
				this.map[i] = null;
			}
		}
	};

	this.updateCoordinates();
};


// The editor's grid
Grid = function (id, map) {
	this.id = id;
	this.map = map;
	this.cX = 0;
	this.cY = 0;

	this.onClick = null;

	// This function draws the grid in which the map is displayed
	this.draw = function () {
		var i, j;
		var str = '<table style="border: 1px solid black; border-collapse: collapse; padding: 0px; margin: 0px">'
			+ '<tr><td style="border: 1px solid black;width: 32px;height:32px">&nbsp;</td>'
			+ '<td style="border: 1px solid black; width:416px" colspan="13" id="map-up">&nbsp;</td>'
			+ '<td style="border: 1px solid black;width: 32px">&nbsp;</td></tr>'
			+ '<tr><td style="border: 1px solid black; height:416px" rowspan="13" id="map-left">&nbsp;</td>'
			+ '<td style="border: 1px solid black;width: 32px;height:32px">&nbsp;</td>';

		for (i = 0; i < 11; i ++) {
			str += '<td style="border: 1px solid black;width: 32px; height: 32px; text-align: center" '
				+ 'id="top-x-' + i + '">&nbsp;</td>';
		}
		str += '<td style="border: 1px solid black;width: 32px;height:32px">&nbsp;</td>'
			+ '<td style="border: 1px solid black; height:416px" rowspan="13" id="map-right">&nbsp;</td>'
			+ '</tr>';

		for (i = 0; i < 11; i ++) {
			str += '<tr><td style="border: 1px solid black;width: 32px; height: 32px; text-align: center" '
				+ 'id="left-y-' + i + '">&nbsp;</td>';
			for (j = 0; j < 11; j ++) {
				str += '<td style="border: 1px solid #7f7f7f; width: 32px; height: '
					+ '32px; text-align:center" id="map-' + j + '-' + i
					+ '" onclick="Editor.editor.mapClick(' + j + ',' + i +')">&nbsp;</td>';
			}
			str += '<td style="border: 1px solid black;width: 32px; height: 32px; text-align:center" '
				+ 'id="right-y-' + i + '">&nbsp;</td></tr>';
		}

		str += '<tr><td style="border: 1px solid black;width: 32px;height:32px">&nbsp;</td>';
		for (i = 0; i < 11; i ++) {
			str += '<td style="border: 1px solid black;width: 32px; height: 32px;text-align:center" '
				+ 'id="bottom-x-' + i + '">&nbsp;</td>';
		}

		str += '<td style="border: 1px solid black;width: 32px;height:32px">&nbsp;</td></tr>'
			+ '<tr><td style="border: 1px solid black;width: 32px; height:32px">&nbsp;</td>'
			+ '<td style="border: 1px solid black; width:416px" colspan="13" id="map-down">&nbsp;</td>'
			+ '<td style="border: 1px solid black;width: 32px">&nbsp;</td></tr>'
			+ '</table>';
		document.getElementById(this.id).innerHTML = str;
		this.drawMap();
	};

	// This function draws the map on the grid
	this.drawMap = function () {
		var i, j, str, x, y;

		// Scroll buttons
		if (this.cX - 5 > this.map.minX) {
			str = '<input type="button" value="<" style="width:100%;height:100%;margin:0"'
				+ ' onclick="Editor.editor.scroll(-1, 0)" />';
		} else {
			str = '&nbsp;';
		}
		document.getElementById('map-left').innerHTML = str;
		if (this.cX + 5 < this.map.maxX) {
			str = '<input type="button" value=">" style="width:100%;height:100%;margin:0"'
				+ ' onclick="Editor.editor.scroll(1, 0)" />';
		} else {
			str = '&nbsp;';
		}
		document.getElementById('map-right').innerHTML = str;
		if (this.cY - 5 > this.map.minY) {
			str = '<input type="button" value="\\/" style="width:100%;height:100%;margin:0"'
				+ ' onclick="Editor.editor.scroll(0, -1)" />';
		} else {
			str = '&nbsp;';
		}
		document.getElementById('map-down').innerHTML = str;
		if (this.cY + 5 < this.map.maxY) {
			str = '<input type="button" value="/\\" style="width:100%;height:100%;margin:0"'
				+ ' onclick="Editor.editor.scroll(0, 1)" />';
		} else {
			str = '&nbsp;';
		}
		document.getElementById('map-up').innerHTML = str;

		// Draw X coordinates
		for (i = 0; i < 11; i ++) {
			x = this.cX + i - 5;
			if (x < this.map.minX || x > this.map.maxX) {
				str = '&nbsp;';
			} else {
				str = '<b>' + x + '</b>';
			}
			document.getElementById('top-x-' + i).innerHTML = 
				document.getElementById('bottom-x-' + i).innerHTML = str;
		}

		// Draw Y coordinates
		for (i = 0; i < 11; i ++) {
			y = this.cY - i + 5;
			if (y < this.map.minY || y > this.map.maxY) {
				str = '&nbsp;';
			} else {
				str = '<b>' + y + '</b>';
			}
			document.getElementById('left-y-' + i).innerHTML = 
				document.getElementById('right-y-' + i).innerHTML = str;
		}

		// Draw contents
		for (i = 0; i < 11; i ++) {
			x = this.cX + i - 5;
			for (j = 0; j < 11; j ++) {
				y = this.cY - j + 5;
				var cell = document.getElementById('map-' + i + '-' + j);

				if (y < this.map.minY || y > this.map.maxY || x < this.map.minX || x > this.map.maxX) {
					cell.innerHTML = '&nbsp;';
					cell.style.backgroundColor = 'black';
					continue;
				}

				var sys = this.map.map['x' + x + 'y' + y];
				if (!sys) {
					cell.innerHTML = '&nbsp;';
					cell.style.backgroundColor = '#3f3f3f';
				} else if (sys.type != 'S') {
					cell.innerHTML = '<b>' + sys.type + '</b>';
					cell.style.backgroundColor = '#afafaf';
					cell.style.color = Grid.nebulaColours[parseInt(sys.type, 10) - 1];
				} else if (sys.alloc == 0) {
					cell.innerHTML = 'T';
					cell.style.backgroundColor = 'black';
					cell.style.color = 'white';
				} else {
					cell.style.color = 'black';
					cell.style.backgroundColor = Grid.allianceColours[sys.alloc - 1];
					cell.innerHTML = sys.spawn ? 'X' : '&nbsp;';
				}
			}
		}
	};

	// This function handle clicks on the map
	this.handleClick = function(i, j) {
		x = this.cX + i - 5;
		y = this.cY - j + 5;

		if (x >= this.map.minX && x <= this.map.maxX
		 && y >= this.map.minY && y <= this.map.maxY && this.onClick) {
			this.onClick(x, y);
		}
	};
};
Grid.allianceColours = [
	'ff0000', '00ff00', '0000ff', '007f7f', '7f007f', 'afaf00', 'ffaf3f', '003f7f'
];
Grid.nebulaColours = [
	'#ff0000', '#ff2f00', '#ff5f00', '#ff7f00'
];


// Toolbox
Toolbox = function (id, map) {
	this.id = id;
	this.map = map;
	this.currentTool = null;

	var drawToolCell = function (id, title, ctxt, cbg, cfg) {
		return str = '<tr><td><table style="width:100%;padding:6px;margin:0px" id="tool-' + id
			+ '" onClick="Editor.editor.selectTool(\'' + id + '\');"><tr>'
			+ '<td style="color:' + cfg + '; background-color: ' + cbg
			+ '; text-align:center;width:32px;height:32px">' + ctxt + '</td>'
			+ '<td style="text-align:center;font-weight:bold">' + title + '</td>'
			+ '</tr></table></td></tr>';
	};

	this.draw = function () {
		var i, str = '<table style="width: 200px; border-style: none">'
			+ '<tr><th style="font-size: 120%">Map drawing tools</th></tr>'
			+ '<tr><th>Nebulae</th></tr>';

		for (i = 1; i < 5; i ++) {
			str += drawToolCell('n' + i, 'Class ' + i + ' nebula', '<b>' + i + '</b>',
				'#afafaf', Grid.nebulaColours[i - 1]);
		}

		str += '<tr><th>Misc</th></tr>'
			+ drawToolCell('tg', 'Target system', 'T', 'black', 'white')
			+ drawToolCell('sp', 'Spawing point', 'X', 'white', 'black')
			+ '<tr><th>Alliances</th></tr>';

		for (i = 1; i <= this.map.alliances; i ++) {
			str += drawToolCell('a' + i, 'Alliance ' + i, '&nbsp;', Grid.allianceColours[i - 1], 'black');
		}

		str += '</table>';

		document.getElementById(this.id).innerHTML = str;
		if (this.currentTool) {
			this.selectTool(this.currentTool);
		}
	};

	this.selectTool = function(tool) {
		if (this.currentTool && document.getElementById('tool-' + this.currentTool)) {
			document.getElementById('tool-' + this.currentTool).style.borderStyle = 'none';
		}

		if (! document.getElementById('tool-' + tool)) {
			this.currentTool = null;
			return;
		}

		with (document.getElementById('tool-' + tool).style) {
			borderColor = 'red';
			borderWidth = '2px';
			borderStyle = 'solid';
		}
		this.currentTool = tool;
	};
};


// Validity check / submit button
CheckAndSubmit = function (id, map) {
	this.id = id;
	this.map = map;
	this.error = "---";

	var mapCheck = function (map) {
		var ax = [];
		var tgc = 0;

		for (var i = 0; i < map.alliances; i ++) {
			ax[i] = [0, 0];
		}

		for (var i = map.minX; i <= map.maxX; i ++) {
			for (var j = map.minY; j <= map.maxY; j ++) {
				var m = map.map['x' + i + 'y' + j];
				if (! m) {
					return 1;
				}
				if (m.type != 'S') {
					continue;
				}
				if (m.alloc == 0) {
					tgc ++;
					continue;
				}
				ax[m.alloc - 1][0] ++;
				if (m.spawn) {
					ax[m.alloc - 1][1] ++;
				}
			}
		}

		if (tgc == 0) {
			return 2;
		}

		var min = -1;
		for (var i = 0; i < map.alliances; i ++) {
			if (ax[i][0] == 0) {
				return 3 + i;
			}
			if (min == -1) {
				min = ax[i][0];
			} else if (ax[i][0] < min) {
				return 19;
			} else if (ax[i][0] > min) {
				return 19 + i;
			}
		}

		min = -1;
		for (var i = 0; i < map.alliances; i ++) {
			if (ax[i][1] == 0) {
				return 11 + i;
			}

			if (min == -1) {
				min = ax[i][1];
			} else if (ax[i][1] < min) {
				return 27;
			} else if (ax[i][1] > min) {
				return 27 + i;
			}
		}

		return 0;
	};

	this.draw = function () {
		if (this.error == '---') {
			this.check();
		}

		var str;
		if (this.error == "") {
			str = '<input type="button" value="Save map" onclick="Editor.editor.submit()" />';
		} else {
			str = '<span style="color:red;font-weight:bold">' + this.error + '</span>';
		}
		document.getElementById(this.id).innerHTML = str;
	};

	this.check = function () {
		var error = "";

		if (map.name.length < 4) {
			error = "Name too short";
		} else if (map.name.length > 32) {
			error = "Name too long";
		} else {
			var i = mapCheck(this.map);
			switch (i) {
				case 0: break;
				case 1: error = "Map has undefined areas"; break;
				case 2: error = "Map has no target areas"; break;
				default:
					if (i < 11) {
						i -= 2;
						error = "Alliance " + i + " has no systems.";
					} else if (i < 19) {
						i -= 10;
						error = "Alliance " + i + " has no spawning points.";
					} else if (i < 27) {
						i -= 18;
						error = "Alliance " + i + " has too many systems.";
					} else if (i < 35) {
						i -= 26;
						error = "Alliance " + i + " has too many spawning points.";
					}
				break;
			}
		}

		if (this.error != error) {
			this.error = error;
			this.draw();
		}
	};
};


// Main editor object
Editor = function (initFrom) {
	var map = new Map(initFrom);
	var components = [];

	var cSub = new CheckAndSubmit('check-and-send', map);
	var tools = new Toolbox('tools', map);
	components.push(tools);

	var grid = new Grid('grid', map);
	grid.onClick = function(x, y) {
		var tool = tools.currentTool;
		if (!tool) {
			return;
		}

		if (tool.match(/^n[1-4]$/)) {
			map.setNebula(x, y, tool.charAt(1));
		} else if (tool == 'tg') {
			map.setTarget(x, y);
		} else if (tool.match(/^a[1-8]$/)) {
			map.setAlliance(x, y, parseInt(tool.charAt(1), 10));
		} else if (tool == 'sp') {
			map.switchSpawnPoint(x, y);
		}

		grid.drawMap();
		cSub.check();
	};
	components.push(grid);

	var c;
	c = new TextInput('name', 'Map name', false, 4, 32, map.name);
	c.onChange = function (value) {
		map.name = value;
		cSub.check();
	};
	components.push(c);

	c = new TextInput('desc', 'Description', true, 0, 0, map.description);
	c.onChange = function (value) {
		map.description = value;
	};
	components.push(c);

	c = new NumericControl('n-alliances', 'Alliances', 2, 8, map.alliances);
	c.onChange = function (value) {
		var old = map.alliances;
		map.alliances = value;
		tools.draw();
		if (value < old) {
			map.removeExtraAlliances();
			grid.drawMap();
		}
		cSub.check();
	};
	components.push(c);

	c = new NumericControl('m-width', 'Map width', 3, 41, map.width);
	c.onChange = function (value) {
		map.width = value;
		map.updateCoordinates();
		grid.drawMap();
		cSub.check();
	};
	components.push(c);

	c = new NumericControl('m-height', 'Map height', 3, 41, map.height);
	c.onChange = function (value) {
		map.height = value;
		map.updateCoordinates();
		grid.drawMap();
		cSub.check();
	};
	components.push(c);
	components.push(cSub);

	this.draw = function () {
		var str = '<h3>Parameters</h3>'
			+ '<table style="width: 100%; border-style: none; margin: 0px; padding: 0px">'
			+ '<tr><td colspan="3" id="name">&nbsp;</td></tr>'
			+ '<tr><td colspan="3" id="desc">&nbsp;</td></tr>'
			+ '<tr><td style="width: 33%" id="n-alliances">&nbsp;</td>'
			+ '<td style="width: 34%" id="m-width">&nbsp;</td>'
			+ '<td style="width: 33%" id="m-height">&nbsp;</td></tr>'
			+ '<tr><td colspan="3" id="check-and-send" style="text-align:center">&nbsp;</td></tr>'
			+ '</table>'
			+ '<h3>Map</h3>'
			+ '<div><div id="tools" style="float:right">&nbsp;</div><div id="grid">&nbsp;</div></div>';
		document.getElementById('mapedit').innerHTML = str;

		for (var i in components) {
			components[i].draw();
		}
	};

	this.scroll = function (dx, dy) {
		grid.cX += dx; grid.cY += dy;
		grid.drawMap();
	};

	this.mapClick = function(x,y) {
		grid.handleClick(x, y);
	};

	this.selectTool = function(tool) {
		tools.selectTool(tool);
	};

	this.submit = function () {
		document.getElementById('sf-name').value = map.name;
		document.getElementById('sf-desc').value = map.description;
		document.getElementById('sf-width').value = map.width;
		document.getElementById('sf-height').value = map.height;
		document.getElementById('sf-alliances').value = map.alliances;

		var ma = [];
		for (var j = map.minY; j <= map.maxY; j ++) {
			var str = '';
			for (var i = map.minX; i <= map.maxX; i ++) {
				var m = map.map['x' + i + 'y' + j];

				str += m.type;
				if (m.type != 'S') {
					continue;
				}
				str += m.alloc;
				if (m.alloc == 0) {
					continue;
				}
				str += m.spawn ? '1' : '0';
			}
			ma.push(str);
		}
		document.getElementById('sf-map').value = ma.join('#');

		document.getElementById('sendform').submit();

	};
};


Editor.editor = new Editor(initMap);
Editor.editor.draw();
