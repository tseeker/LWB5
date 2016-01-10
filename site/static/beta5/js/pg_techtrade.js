/* TechTrade component
 *
 * Controls the whole page.
 */
TechTrade = function (initialData) {
	Component.apply(this, [false]);
	TechTrade.main = this;

	this.newEvent('ListLoaded');

	this.newSlot('listReceived');
	this.newSlot('dataReceived');
	this.newSlot('updateData');

	this.timer = new Component.Timer(60 * 1000, true);
	this.getInfo = new Component.Ajax('getGeneralInfo');
	this.getList = new Component.Ajax('getTechList');

	this.accessDenied = new TechTrade.AccessDenied();
	this.techPage = new TechTrade.Page();

	this.getInfo.bindEvent('Returned', 'dataReceived', this);
	this.getList.bindEvent('Returned', 'listReceived', this);
	this.timer.bindEvent('Tick', 'updateData', this);
	this.bindEvent('ListLoaded', 'drawAll', this.techPage);

	this.page = null;
	this.list = null;

	this.show();
	this.dataReceived(initialData);
	this.timer.start();
	this.getList.call();
};
TechTrade.prototype = new Component;
TechTrade.prototype.listReceived = function (data) {
	var list = [];
	var spData = data.split('\n');
	var line;
	while (line = spData.shift()) {
		var spLine = line.split('#');
		var id = spLine.shift();
		var nDeps = parseInt(spLine.shift(), 10);
		var deps = [];
		for (var i = 0; i < nDeps; i ++) {
			deps.push(spLine.shift());
		}
		list[id] = {
			dependencies: deps,
			name: spLine.join('#')
		};
	}
	this.list = list;
	this.onListLoaded();
	this.switchPage(this.page);
};
TechTrade.prototype.updateData = function () {
	this.getInfo.call();
};
TechTrade.prototype.dataReceived = function (data) {
	var spData = data.split('#');
	var privileges = parseInt(spData[1], 10);
	this.privileges = privileges;
	this.hasRequests = (spData[2] == '1');
	this.alliance = spData[3];
	this.vacation = (spData[4] == '1');
	this.switchPage(spData[0]);
};
TechTrade.prototype.switchPage = function (page) {
	this.page = page;
	if (page == 'NoAccess') {
		this.removeChild(this.techPage);
		this.addChild(this.accessDenied);
	} else if (this.list) {
		this.removeChild(this.accessDenied);
		this.techPage.setPage(page);
		if (this.hasChild(this.techPage)) {
			this.techPage.menu.drawAll();
		} else {
			this.addChild(this.techPage);
		}
	}
};


/** TechTrade.computeDependencies
 *
 * Method to compute who can send what to whom
 */
TechTrade.computeDependencies = function (techList) {
	var statusList = [];

	for (var i in techList) {
		var pTechStat = techList[i];
		var pStatus = {
			canSend: [],
			canReceive: [],
			ok: false
		};
		statusList[i] = pStatus;

		// On vacation or restrained, we're done.
		if (pTechStat.onVacation || pTechStat.restriction || !pTechStat.lastSubmission) {
			continue;
		}
		pStatus.ok = true;

		// Compute technologies the player can receive from others
		var satisfied = [];
		for (var t in TechTrade.main.list) {
			if (pTechStat.techs[t]) {
				continue;
			}

			var deps = TechTrade.main.list[t].dependencies;
			var canReceive = true;
			for (var di in deps) {
				var d = deps[di];
				if (pTechStat.techs[d] != 'I' && pTechStat.techs[d] != 'L') {
					canReceive = false;
					break;
				}
			}
			if (! canReceive) {
				continue;
			}

			satisfied.push(t);
		}

		// Check players who could send these techs
		for (var ti in satisfied) {
			var t = satisfied[ti];
			for (var si in techList) {
				if (!techList[si].techs[t] || techList[si].techs[t] == 'F') {
					continue;
				}
				pStatus.canReceive.push({
					tech: t,
					player: si
				});
			}
		}
	}

	for (var i in statusList) {
		var pStatus = statusList[i];

		for (var ti in pStatus.canReceive) {
			var t = pStatus.canReceive[ti];
			if (statusList[t.player].ok) {
				statusList[t.player].canSend.push({
					tech: t.tech,
					player: i
				});
			}
		}
	}

	// DEBUG
//	alert(debugObj(statusList));

	return statusList;
};

function debugObj(obj) {
	var str = '{\n';
	for (var i in obj) {
		str += i + ': ' + (typeof obj[i] == 'object' ? debugObj(obj[i]) : obj[i]) + '\n';
	}
	return str + '}\n';
};


/* TechTrade.AccessDenied component
 *
 * Displays the "access denied" page.
 */
TechTrade.AccessDenied = function () {
	Component.apply(this, [false]);
};
TechTrade.AccessDenied.prototype = new Component;
TechTrade.AccessDenied.prototype.getDocID = function () { return 'page-contents'; };
TechTrade.AccessDenied.prototype.draw = function () {
	var e = this.getElement();
	if (!e) {
		return;
	}
	e.innerHTML = '<h1>' + TechTrade.AccessDenied.title + '</h1><p>'
		+ TechTrade.AccessDenied.mainText + '<br/>'
		+ (TechTrade.main.alliance != ''
			? TechTrade.AccessDenied.allianceTxt
			: TechTrade.AccessDenied.noAllianceTxt)
		+ '</p>';
};


/* TechTrade.Page component
 *
 * Handles the main technology trading page, including the menu and contents.
 */
TechTrade.Page = function () {
	Component.apply(this, [false]);

	this.newSlot('showPage');

	this.menu = new TechTrade.Menu();
	this.addChild(this.menu);
	this.menu.bindEvent('Selected', 'showPage', this);

	this.pages = {
		Submit:		new TechTrade.Page.Main(),
		Req:		new TechTrade.Page.Request(),
		ViewList:	new TechTrade.Page.List(),
		SOrders:	new TechTrade.Page.SetOrders(),
		VOrders:	new TechTrade.Page.ViewOrders()
	};
	this.cPage = null;
};
TechTrade.Page.prototype = new Component;
TechTrade.Page.prototype.getDocID = function () { return 'page-contents'; };
TechTrade.Page.prototype.setPage = function (page) {
	this.menu.select(page);
};
TechTrade.Page.prototype.showPage = function (page) {
	if (this.cPage) {
		this.removeChild(this.cPage);
	}
	TechTrade.main.page = page;
	this.cPage = this.pages[page];
	if (this.cPage) {
		this.addChild(this.cPage);
		this.cPage.show();
	}
};
TechTrade.Page.prototype.draw = function () {
	var e = this.getElement();
	if (!e) {
		return;
	}

	if (TechTrade.main.list) {
		e.innerHTML = '<form style="margin:0px;padding:0px">'
			+ '<div style="text-align:center;width:100%; margin: 0px 0px 10px 0px" '
			+ 'id="page-menu">&nbsp;</div><div style="width:100%" id="page-panel">&nbsp;</div></form>';
	} else {
		e.innerHTML = '<h1 style="text-align:center;margin: 15px 0px">' + TechTrade.Page.loadingText
			+ '</h1>';
	}
};


/* TechTrade.Menu component
 *
 * Displays the menu at the top of the page
 */
TechTrade.Menu = function () {
	Component.apply(this, [false]);

	this.selected = null;
	this.newEvent('Selected', false);
	this.bindEvent('Selected', 'draw');
};
TechTrade.Menu.prototype = new Component;
TechTrade.Menu.prototype.getDocID = function () { return 'page-menu'; };
TechTrade.Menu.prototype.select = function (page) {
	if (page != this.selected) {
		this.selected = page;
		this.onSelected(page);
	}
};
TechTrade.Menu.prototype.draw = function () {
	var e = this.getElement();
	if (!e) {
		return;
	}

	var entries = [];
	for (var i = 0; i < TechTrade.Menu.entries.length ; i ++) {
		if (TechTrade.main.privileges < TechTrade.Menu.minPriv[i]
				|| (! TechTrade.main.hasRequests && TechTrade.Menu.needReq[i]) ) {
			continue;
		}
		var str;
		if (TechTrade.Menu.entries[i] == this.selected) {
			str = '<b>';
		} else {
			str = '<a href="#" onclick="Component.list[' + this.id + '].select(\''
				+ TechTrade.Menu.entries[i] + '\'); return false">';
		}
		str += TechTrade.Menu.text[i] + (TechTrade.Menu.entries[i] == this.selected ? '</b>' : '</a>');
		entries.push(str);
	}

	e.innerHTML = entries.join(' - ');
};
TechTrade.Menu.entries = [ 'Submit', 'Req', 'ViewList', 'SOrders', 'VOrders' ];
TechTrade.Menu.minPriv = [ 1, 2, 3, 4, 4 ];
TechTrade.Menu.needReq = [ false, true, false, false, false ];


/* TechTrade.Page.Main component
 *
 * Handles the tech list submission / player orders viewing page.
 */
TechTrade.Page.Main = function () {
	Component.apply(this, [false]);

	this.newSlot('preparePage');
	this.newSlot('dataReceived');

	this.detachEvent('Show', 'drawAll');
	this.bindEvent('Show', 'preparePage');

	this.loadPage = new Component.Ajax('getSubmitPage');
	this.sendList = new Component.Ajax('submitList');
	this.ajaxSendOffer = new Component.Ajax('sendOffer');
	this.loadPage.bindEvent('Returned', 'dataReceived', this);
	this.sendList.bindEvent('Returned', 'dataReceived', this);
	this.ajaxSendOffer.bindEvent('Returned', 'dataReceived', this);
};
TechTrade.Page.Main.prototype = new Component;
TechTrade.Page.Main.prototype.dataReceived = function (data) {
	if (data == 'FU') {
		TechTrade.main.getInfo.call();
		return;
	}

	var toStore = {};
	var spData = data.split('\n');
	var line;

	// Submission data
	line = spData.shift().split('#');
	toStore.canSubmit = (line.shift() == 1);
	if (line[0] != '') {
		toStore.prevSubmission = parseInt(line.shift(), 10);
	} else {
		toStore.prevSubmission = null;
		line.shift();
	}
	if (line[0] != '') {
		toStore.nextSubmission = parseInt(line.shift(), 10);
	} else {
		toStore.nextSubmission = null;
	}

	// Send order
	line = spData.shift();
	if (line == '-') {
		toStore.sendOrder = null;
	} else {
		toStore.sendOrder = {};
		line = line.split('#');
		toStore.sendOrder.orderedOn = parseInt(line.shift(), 10);
		if (line[0] == '') {
			toStore.sendOrder.obeyedOn = null;
			line.shift();
		} else {
			toStore.sendOrder.obeyedOn = parseInt(line.shift(), 10);
		}
		toStore.sendOrder.canSend = (line.shift() == '1');
		toStore.sendOrder.what = line.shift();
		toStore.sendOrder.playerID = line.shift();
		toStore.sendOrder.playerName = line.join('#');
	}

	// Receive order
	line = spData.shift();
	if (line == '-') {
		toStore.recOrder = null;
	} else {
		toStore.recOrder = {};
		line = line.split('#');
		toStore.recOrder.orderedOn = parseInt(line.shift(), 10);
		if (line[0] == '') {
			toStore.recOrder.obeyedOn = null;
			line.shift();
		} else {
			toStore.recOrder.obeyedOn = parseInt(line.shift(), 10);
		}
		toStore.recOrder.what = line.shift();
		toStore.recOrder.playerID = line.shift();
		toStore.recOrder.playerName = line.join('#');
	}

	// Draw page
	this.data = toStore;
	this.drawAll();
};
TechTrade.Page.Main.prototype.preparePage = function () {
	this.data = null;
	this.loadPage.call();
	this.drawAll();
};
TechTrade.Page.Main.prototype.getDocID = function () { return "page-panel"; };
TechTrade.Page.Main.prototype.draw = function () {
	var e = this.getElement();
	if (!e || TechTrade.main.page != 'Submit') {
		return;
	}

	if (this.data) {
		e.innerHTML = '<table style="border-style:none; margin: 0px; padding: 0px;width: 100%">'
			+ '<tr><td style="width:2%">&nbsp;</td><td style="width: 48%; vertical-align:top"><h3>'
			+ TechTrade.Page.Main.submitTitle + '</h3>' + this.drawSubmit() + '</td>'
			+ '<td style="width: 48%; vertical-align:top"><h3>' + TechTrade.Page.Main.ordersTitle
			+ '</h3><p>' + this.drawOrders() + '</p></td><td style="width:2%">&nbsp;</td></tr>';
	} else {
		e.innerHTML = '<h3 style="text-align:center; margin: 10px 0px">' + TechTrade.Page.subLoadingText
			+ '</h3>';
	}
};
TechTrade.Page.Main.prototype.drawSubmit = function () {
	var str = '<p>';
	if (this.data.prevSubmission) {
		str += TechTrade.Page.Main.lastSubText.replace(/__LS__/, formatDate(this.data.prevSubmission));
	} else {
		str += TechTrade.Page.Main.noLastSubText;
	}
	if (this.data.canSubmit) {
		str += '</p><p style="margin:0px;padding: 5px 0px;text-align:center">'
			+ '<input type="button" id="submit-tech-list" onclick="Component.list[\'' + this.id
			+ '\'].submitList(); return false" value="' + TechTrade.Page.Main.submitButtonText
			+ '" ' + (TechTrade.main.vacation ? ' disabled="disabled"' : '') + '/></p>';
	} else {
		str += '<br/>' + TechTrade.Page.Main.nextSubText.replace(/__NS__/, formatDate(this.data.nextSubmission))
			+ '</p>';
	}

	return str;
};
TechTrade.Page.Main.prototype.drawOrders = function () {
	if (!(this.data.sendOrder || this.data.recOrder)) {
		return TechTrade.Page.Main.noOrdersText;
	}

	var str = '', otext;
	if (this.data.sendOrder) {
		otext = TechTrade.Page.Main.sendOrder;
		otext = otext.replace(/__TN__/, TechTrade.main.list[this.data.sendOrder.what].name);
		otext = otext.replace(/__TI__/, this.data.sendOrder.what);
		otext = otext.replace(/__PI__/, this.data.sendOrder.playerID);
		otext = otext.replace(/__PN__/, this.data.sendOrder.playerName);
		otext = otext.replace(/__OR__/, formatDate(this.data.sendOrder.orderedOn));
		str += '<h4>' + TechTrade.Page.Main.sendOrderTitle + '</h4><p>' + otext;
		if (this.data.sendOrder.obeyedOn) {
			otext = TechTrade.Page.Main.sendOrderObeyed;
			otext = otext.replace(/__OE__/, formatDate(this.data.sendOrder.obeyedOn));
			str += '<br/>' + otext;
		} else {
			str += '</p><p style="margin:0px;padding: 5px 0px;text-align:center">'
				+ '<input type="button" id="send-tech-offer" onclick="Component.list[\'' + this.id
				+ '\'].sendOffer(); return false" value="' + TechTrade.Page.Main.sendButtonText
				+ '" ' + (this.data.sendOrder.canSend ? '' : ' disabled="disabled"') + '/></p>';
		}
		str += '</p>';
	}

	if (this.data.recOrder) {
		otext = TechTrade.Page.Main.recOrder;
		otext = otext.replace(/__TN__/, TechTrade.main.list[this.data.recOrder.what].name);
		otext = otext.replace(/__TI__/, this.data.recOrder.what);
		otext = otext.replace(/__PI__/, this.data.recOrder.playerID);
		otext = otext.replace(/__PN__/, this.data.recOrder.playerName);
		otext = otext.replace(/__OR__/, formatDate(this.data.recOrder.orderedOn));
		str += '<h4>' + TechTrade.Page.Main.recOrderTitle + '</h4><p>' + otext;
		if (this.data.recOrder.obeyedOn) {
			str += TechTrade.Page.Main.recOrderObeyed.replace(/__OE__/, formatDate(
				this.data.recOrder.obeyedOn
			));
		}
		str += '.</p>';
	}

	return str;
};
TechTrade.Page.Main.prototype.submitList = function () {
	var e = document.getElementById('submit-tech-list');
	if (!e) {
		return;
	}
	e.disabled = true;
	this.sendList.call();
};
TechTrade.Page.Main.prototype.sendOffer = function () {
	var e = document.getElementById('send-tech-offer');
	if (!e) {
		return;
	}
	e.disabled = true;
	this.ajaxSendOffer.call();
};


/** TechTrade.Page.Request component
 *
 * Handles the requests page
 */
TechTrade.Page.Request = function () {
	Component.apply(this, [false]);

	this.newSlot('preparePage');
	this.newSlot('dataReceived');

	this.detachEvent('Show', 'drawAll');
	this.bindEvent('Show', 'preparePage');

	this.addSelector = new Component.Form.DropDown('', TechTrade.Page.Request.noSelection);
	this.addChild(this.addSelector);
	this.addSelector.bindEvent('Selected', 'drawAll', this);

	this.loadPage = new Component.Ajax('getRequestsPage');
	this.submitRequests = new Component.Ajax('submitRequests');
	this.loadPage.bindEvent('Returned', 'dataReceived', this);
	this.submitRequests.bindEvent('Returned', 'dataReceived', this);
};
TechTrade.Page.Request.prototype = new Component;
TechTrade.Page.Request.prototype.getDocID = function () { return "page-panel"; };
TechTrade.Page.Request.prototype.draw = function () {
	var e = this.getElement();
	if (!e || TechTrade.main.page != 'Req') {
		return;
	}

	if (this.data) {
		var str = '<h3>' + TechTrade.Page.Request.title + '</h3>';
		if (this.data.requests.length) {
			str += '<ol>';
			for (var i = 0; i < this.data.requests.length; i ++) {
				str += this.drawRequestLine(i);
			}
			str += '</ol>';
		} else {
			str += '<p>' + TechTrade.Page.Request.noReqText + '</p>';
		}
		str += '<input type="button" style="margin: 0px 0px 10px 70px" value="'
			+ TechTrade.Page.Request.submitText + '" onclick=Component.list[\'' + this.id
			+ '\'].submit();return false" id="submit-requests"'
			+ ((TechTrade.main.vacation || this.data.originalRequests == this.data.requests.join('#'))
				? ' disabled="disabled"'
				: '')
			+ '/>';
		if (this.data.originalRequests != this.data.requests.join('#')) {
		}
		if (this.data.canRequest.length && this.data.requests.length < 5) {
			str += '<p>' + TechTrade.Page.Request.addRequest + ' <span id="'
				+ this.addSelector.getDocID() + '">&nbsp;</span> '
				+ this.drawAddLinks() + '</p>';
		} else if (this.data.canRequest.length) {
			str += '<p>' + TechTrade.Page.Request.fiveRequests + '</p>';
		} else {
			str += '<p>' + TechTrade.Page.Request.noAvailableTechs + '</p>';
		}
		e.innerHTML = str;
	} else {
		e.innerHTML = '<h3 style="text-align:center; margin: 10px 0px">' + TechTrade.Page.subLoadingText
			+ '</h3>';
	}
};
TechTrade.Page.Request.prototype.preparePage = function () {
	this.data = null;
	this.loadPage.call();
	this.drawAll();
};
TechTrade.Page.Request.prototype.dataReceived = function (data) {
	if (data == 'FU') {
		TechTrade.main.getInfo.call();
		return;
	}

	var spData = data.split('\n');
	var toStore = {};

	toStore.requests = (spData[0] == '' ? [] : spData[0].split('#'));
	toStore.canRequest = (spData[1] == '' ? [] : spData[1].split('#'));
	toStore.originalRequests = spData[0];

	this.data = toStore;
	this.updateSelector();
	this.drawAll();
};
TechTrade.Page.Request.prototype.updateSelector = function () {
	this.addSelector.clearOptions();
	var reqStr = '#' + this.data.requests.join('##') + '#';
	var canRequest = [];
	for (var i in this.data.canRequest) {
		if (reqStr.indexOf('#' + this.data.canRequest[i] + '#') == -1) {
			canRequest.push(this.data.canRequest[i]);
		}
	}
	canRequest.sort(function (a, b) {
		var ta = TechTrade.main.list[a].name.toLowerCase(),
			tb = TechTrade.main.list[b].name.toLowerCase();
		return (ta == tb) ? 0 : (ta > tb ? 1 : -1);
	});
	for (var i in canRequest) {
		this.addSelector.appendOption(
			canRequest[i], TechTrade.main.list[canRequest[i]].name
		);
	}
};
TechTrade.Page.Request.prototype.drawAddLinks = function () {
	var v = this.addSelector.selected;
	if (v == '') {
		return '';
	}
	return '<a href="manual?p=tech_' + v + '" target="_blank">' + TechTrade.Page.Request.addRequestManual
		+ '</a> - <a href="#" onclick="Component.list[\'' + this.id + '\'].addRequest();return false">'
		+ TechTrade.Page.Request.addRequestAdd + '</a>';
};
TechTrade.Page.Request.prototype.addRequest = function () {
	var v = this.addSelector.selected;
	if (v == '' || this.data.requests.length == 5) {
		this.drawAll();
		return;
	}
	this.data.requests.push(v);
	this.updateSelector();
	this.drawAll();
};
TechTrade.Page.Request.prototype.drawRequestLine = function (index) {
	var tech = this.data.requests[index];
	var str = '<li><a href="manual?p=tech_' + tech + '" target="_blank">'
		+ TechTrade.main.list[tech].name + '</a> (<a href="#" onclick="Component.list[\'' + this.id
		+ '\'].removeRequest(' + index + ');return false">' + TechTrade.Page.Request.delRequest
		+ '</a>';
	if (index > 0) {
		str += ' - <a href="#" onclick="Component.list[\'' + this.id + '\'].increasePriority('
			+ index + ');return false">' + TechTrade.Page.Request.increasePriority + '</a>';
	}
	if (index < 4 && this.data.requests.length > index + 1) {
		str += ' - <a href="#" onclick="Component.list[\'' + this.id + '\'].decreasePriority('
			+ index + ');return false">' + TechTrade.Page.Request.decreasePriority + '</a>';
	}
	str += ')';
	return str;
};
TechTrade.Page.Request.prototype.increasePriority = function (index) {
	var v1 = this.data.requests[index];
	this.data.requests[index] = this.data.requests[index - 1];
	this.data.requests[index - 1] = v1;
	this.drawAll();
};
TechTrade.Page.Request.prototype.decreasePriority = function (index) {
	var v1 = this.data.requests[index];
	this.data.requests[index] = this.data.requests[index + 1];
	this.data.requests[index + 1] = v1;
	this.drawAll();
};
TechTrade.Page.Request.prototype.removeRequest = function (index) {
	this.data.requests.splice(index, 1);
	this.updateSelector();
	this.drawAll();
};
TechTrade.Page.Request.prototype.submit = function () {
	var e = document.getElementById('submit-requests');
	if (!e) {
		return;
	}
	e.disabled = true;
	this.submitRequests.call(this.data.requests.join('#'));
};


/** TechTrade.Page.List component
 *
 * Displays the alliance-wide technology list
 */
TechTrade.Page.List = function () {
	Component.apply(this, [false]);

	this.newSlot('preparePage');
	this.newSlot('dataReceived');

	this.newSlot('pppChanged');
	this.newSlot('tppChanged');

	this.detachEvent('Show', 'drawAll');
	this.bindEvent('Show', 'preparePage');

	this.md5 = null;
	this.data = null;
	this.xCoord = this.yCoord = 0;

	this.pppSelector = new Component.Form.DropDown('', '???');
	this.addChild(this.pppSelector);
	for (var i = 2; i < 11; i ++) {
		this.pppSelector.appendOption(i, i);
	}
	this.pppSelector.bindEvent('Selected', 'pppChanged', this);

	this.tppSelector = new Component.Form.DropDown('', '???');
	this.addChild(this.tppSelector);
	for (var i = 1; i < 9; i ++) {
		this.tppSelector.appendOption(i * 10, i * 10);
	}
	this.tppSelector.bindEvent('Selected', 'tppChanged', this);

	this.loadPage = new Component.Ajax('getAllianceList');
	this.loadPage.bindEvent('Returned', 'dataReceived', this);
	this.sendOptions = new Component.Ajax('setListOptions');
};
TechTrade.Page.List.prototype = new Component;
TechTrade.Page.List.prototype.getDocID = function () { return "page-panel"; };
TechTrade.Page.List.prototype.draw = function () {
	var e = this.getElement();
	if (!e || TechTrade.main.page != 'ViewList') {
		return;
	}

	if (this.data) {
		if (this.forceRedraw || !document.getElementById('alliance-list-grid')) {
			e.innerHTML = this.genGrid();
			this.forceRedraw = false;
		}
		this.drawGridContents();
	} else {
		e.innerHTML = '<h3 style="text-align:center; margin: 10px 0px">' + TechTrade.Page.subLoadingText
			+ '</h3>';
	}
};
TechTrade.Page.List.prototype.preparePage = function () {
	this.loadPage.call(this.md5);
	this.drawAll();
};
TechTrade.Page.List.prototype.dataReceived = function (data) {
	if (data == 'FU') {
		TechTrade.main.getInfo.call();
		return;
	} else if (data == 'NC') {
		return;
	}

	var spLine, nTechs;
	var spData = data.split('\n');
	var newMD5 = spData.shift();

	spLine = spData.shift().split('#');
	if (! this.techsPerPage) {
		this.techsPerPage = parseInt(spLine[0], 10);
		this.playersPerPage = parseInt(spLine[1], 10);
	}

	var nPlayers = parseInt(spData.shift(), 10);
	var toStore = [];

	for (var i = 0; i < nPlayers; i ++) {
		spLine = spData.shift().split('#');
		var pId = spLine.shift();
		var pData = {};

		pData.onVacation = (spLine.shift() == '1');
		pData.lastSubmission = parseInt(spLine.shift(), 10);
		pData.restriction = (spLine.shift() != '0');
		nTechs = parseInt(spLine.shift(), 10);
		pData.name = spLine.join('#');
		pData.techs = [];

		for (var j = 0; j < nTechs; j ++) {
			spLine = spData.shift().split('#');
			pData.techs[spLine[0]] = spLine[1];
		}

		spLine = spData.shift();
		if (spLine == '') {
			pData.requests = [];
		} else {
			pData.requests = spLine.split('#');
		}

		toStore[pId] = pData;
	}

	this.md5 = newMD5;
	this.data = toStore;

	this.plCache = [];
	for (j in this.data) {
		this.plCache.push(j);
	}
	var me = this;
	this.plCache.sort(function (a, b) {
		var pa = me.data[a].name.toLowerCase(), pb = me.data[b].name.toLowerCase();
		return (pa > pb ? 1 : (pa < pb ? -1 : 0));
	});

	if (this.playersPerPage > this.plCache.length) {
		this.playersPerPage = this.plCache.length;
	}
	if (this.playersPerPage + this.xCoord > this.plCache.length) {
		this.xCoord = this.plCache.length - (this.playersPerPage - 1);
	}

	this.tppSelector.select(this.techsPerPage);
	this.pppSelector.select(this.playersPerPage);

	this.drawAll();
};
TechTrade.Page.List.prototype.genGrid = function () {
	var str0 = '<table style="border-style:none;border-width:0px;width:97%;margin:0px 1%;border-collapse:'
		+ 'collapse" id="alliance-list-grid"><tr><td style="padding:10px 0px;text-align:center;'
		+ 'vertical-align:middle" colspan="' + (this.playersPerPage + 3) + '">'
		+ TechTrade.Page.List.playersPerPage + ' <span id="' + this.pppSelector.getDocID()
		+ '">&nbsp;</span> - ' + TechTrade.Page.List.techsPerPage + ' <span id="' + this.tppSelector.getDocID()
		+ '">&nbsp;</span>'
		+ '</td></tr><tr><td style="border:1px solid #7f7f7f;width:32px;vertical-align:middle;text-align:center"'
		+ ' rowspan="' + (this.techsPerPage + 6) + '" id="list-left" onclick="Component.list[\'' + this.id
		+ '\'].moveLeft()">&nbsp;</td><th style="border: 1px solid #7f7f7f;width:250px">'
		+ TechTrade.Page.List.playerHdr + '</th>';
	var str1 = '<td style="border:1px solid #7f7f7f;width:32px;vertical-align:middle;text-align:center" rowspan="'
		+ (this.techsPerPage + 6) + '" id="list-right" onclick="Component.list[\'' + this.id
		+ '\'].moveRight()">&nbsp;</td></tr><tr><th style="border: 1px solid #7f7f7f">'
		+ TechTrade.Page.List.lastSubHdr + '</th>';
	var str2 = '</tr><tr><th style="border: 1px solid #7f7f7f">' + TechTrade.Page.List.vacationHdr + '</th>';
	var str3 = '</tr><tr><th style="border: 1px solid #7f7f7f">' + TechTrade.Page.List.restrainedHdr + '</th>';
	var str4 = '</tr><tr><td colspan="' + (this.playersPerPage + 1) + '" style="border:1px solid #7f7f7f;'
		+ 'text-align:center;height:32px;vertical-align:middle" id="list-up" onclick="Component.list[\''
		+ this.id + '\'].moveUp()">&nbsp;</td></tr>';
	for (var i = 0; i < this.playersPerPage; i ++) {
		str0 += '<th style="border:1px solid #7f7f7f" id="player-hdr-' + i + '">&nbsp;</th>';
		str1 += '<td style="border:1px solid #7f7f7f;text-align:center" id="last-sub-hdr-' + i + '">&nbsp;</td>';
		str2 += '<td style="border:1px solid #7f7f7f;text-align:center" id="vac-hdr-' + i + '">&nbsp;</td>';
		str3 += '<td style="border:1px solid #7f7f7f;text-align:center" id="res-hdr-' + i + '">&nbsp;</td>';
	}
	for (var i = 0; i < this.techsPerPage; i ++) {
		str4 += '<tr><td style="border:1px solid #7f7f7f" id="tech-hdr-' + i + '">&nbsp;</td>';
		for (var j = 0; j < this.playersPerPage; j ++) {
			str4 += '<td style="border:1px solid #3f3f3f;text-align:center" id="ptech-' + j + '-'
				+ i + '">&nbsp;</td>';
		}
		str4 += '</tr>';
	}
	str4 += '<tr><td colspan="' + (this.playersPerPage + 1) + '" style="border:1px solid #7f7f7f;text-align:'
		+ 'center;height:32px;vertical-align:middle" id="list-down" onclick="Component.list[\'' + this.id
		+ '\'].moveDown()">&nbsp;</td></tr>';
	return str0 + str1 + str2 + str3 + str4 + '</table>';
};
TechTrade.Page.List.prototype.drawGridContents = function () {
	if (!document.getElementById('alliance-list-grid') || ! TechTrade.main.list) {
		return;
	}

	var i, j;

	if (! this.tlCache) {
		this.tlCache = [];
		for (j in TechTrade.main.list) {
			this.tlCache.push(j);
		}
	}

	if (this.yCoord > 0) {
		document.getElementById('list-up').innerHTML = '<img src="' + staticurl
			+ '/beta5/pics/up_' + color + '.gif" alt="/\\" />';
	} else {
		document.getElementById('list-up').innerHTML = '&nbsp;';
	}
	if (this.yCoord + this.techsPerPage < this.tlCache.length) {
		document.getElementById('list-down').innerHTML = '<img src="' + staticurl
			+ '/beta5/pics/down_' + color + '.gif" alt="\\/" />';
	} else {
		document.getElementById('list-down').innerHTML = '&nbsp;';
	}

	if (this.xCoord > 0) {
		document.getElementById('list-left').innerHTML = '<img src="' + staticurl
			+ '/beta5/pics/left_' + color + '.gif" alt="<<" />';
	} else {
		document.getElementById('list-left').innerHTML = '&nbsp;';
	}
	if (this.xCoord + this.playersPerPage < this.plCache.length) {
		document.getElementById('list-right').innerHTML = '<img src="' + staticurl
			+ '/beta5/pics/right_' + color + '.gif" alt=">>" />';
	} else {
		document.getElementById('list-right').innerHTML = '&nbsp;';
	}

	for (i = this.yCoord, j = 0; i < this.yCoord + this.techsPerPage; i ++, j ++) {
		document.getElementById('tech-hdr-' + j).innerHTML = TechTrade.main.list[this.tlCache[i]].name;
	}
	for (i = this.xCoord, j = 0; i < this.xCoord + this.playersPerPage; i ++, j ++) {
		if (! this.plCache[i]) {
			document.getElementById('player-hdr-' + j).innerHTML = '&nbsp;';
			document.getElementById('last-sub-hdr-' + j).innerHTML = '&nbsp;';
			document.getElementById('vac-hdr-' + j).innerHTML = '&nbsp;';
			document.getElementById('res-hdr-' + j).innerHTML = '&nbsp;';
			for (k = 0; k < this.techsPerPage; k ++) {
				with (document.getElementById('ptech-' + j + '-' + k)) {
					innerHTML = '&nbsp';
					backgroundColor = color = '#000000';
				}
			}
			continue;
		}

		document.getElementById('player-hdr-' + j).innerHTML = this.data[this.plCache[i]].name;
		var d = this.data[this.plCache[i]].lastSubmission;
		document.getElementById('last-sub-hdr-' + j).innerHTML = d ? formatDate(d) : '-';
		document.getElementById('vac-hdr-' + j).innerHTML = TechTrade.Page.List.boolText[
			this.data[this.plCache[i]].onVacation ? 1 : 0
		];
		document.getElementById('res-hdr-' + j).innerHTML = TechTrade.Page.List.boolText[
			this.data[this.plCache[i]].restriction ? 1 : 0
		];

		var k, l;
		for (k = this.yCoord, l = 0; k < this.yCoord + this.techsPerPage; k ++, l ++) {
			var cell = document.getElementById('ptech-' + j + '-' + l);
			var tStat = this.data[this.plCache[i]].techs[this.tlCache[k]];

			if (!tStat) {
				var z;
				for (z = 0; z < this.data[this.plCache[i]].requests.length; z ++) {
					if (this.data[this.plCache[i]].requests[z] == this.tlCache[k]) {
						cell.style.backgroundColor = '#ff0000';
						cell.style.color = '#ffffff';
						cell.innerHTML = TechTrade.Page.List.requested + ' (#' + (z + 1) + ')';
						break;
					}
				}
				if (z == this.data[this.plCache[i]].requests.length) {
					cell.style.backgroundColor = '#000000';
					cell.style.color = '#7f7f7f';
					cell.innerHTML = this.data[this.plCache[i]].lastSubmission ? '-' : '?';
				}
				continue;
			}

			var col, bgCol, contents;
			switch (tStat) {
				case 'N':
					col = '#000000'; bgCol = '#bfbf7f'; contents = TechTrade.Page.List.techStatus[0];
					break;
				case 'F':
					col = '#ff0000'; bgCol = '#8fbf8f'; contents = TechTrade.Page.List.techStatus[1];
					break;
				case 'I':
					col = '#0000ff'; bgCol = '#5fbf5f'; contents = TechTrade.Page.List.techStatus[2];
					break;
				case 'L':
					col = '#0000ff'; bgCol = '#5fbf5f'; contents = TechTrade.Page.List.techStatus[3];
					break;
			}
			cell.style.backgroundColor = bgCol;
			cell.style.color = col;
			cell.innerHTML = contents;
		}
	}

};
TechTrade.Page.List.prototype.moveUp = function () {
	if (this.yCoord > 0) {
		this.yCoord --;
		this.drawGridContents();
	}
};
TechTrade.Page.List.prototype.moveDown = function () {
	if (this.tlCache && this.yCoord + this.techsPerPage < this.tlCache.length) {
		this.yCoord ++;
		this.drawGridContents();
	}
};
TechTrade.Page.List.prototype.moveLeft = function () {
	if (this.xCoord > 0) {
		this.xCoord --;
		this.drawGridContents();
	}
};
TechTrade.Page.List.prototype.moveRight = function () {
	if (this.plCache && this.xCoord + this.playersPerPage < this.plCache.length) {
		this.xCoord ++;
		this.drawGridContents();
	}
};
TechTrade.Page.List.prototype.pppChanged = function (value) {
	var v = parseInt(value, 10);
	if (this.playersPerPage == v) {
		return;
	}
	if (v > this.plCache.length) {
		this.pppSelector.select(this.plCache.length);
		return;
	}
	this.playersPerPage = v;
	if (this.playersPerPage + this.xCoord > this.plCache.length) {
		this.xCoord = this.plCache.length - this.playersPerPage;
	}
	this.sendOptions.call(this.playersPerPage, this.techsPerPage);
	this.forceRedraw = true;
	this.drawAll();
};
TechTrade.Page.List.prototype.tppChanged = function (value) {
	if (this.techsPerPage == parseInt(value, 10)) {
		return;
	}
	this.techsPerPage = parseInt(value, 10);
	if (this.techsPerPage + this.yCoord > this.tlCache.length) {
		this.yCoord = this.tlCache.length - this.techsPerPage;
	}
	this.forceRedraw = true;
	this.sendOptions.call(this.playersPerPage, this.techsPerPage);
	this.drawAll();
};


/* TechTrade.Page.ViewOrders component
 *
 * Handles the list of current orders
 */
TechTrade.Page.ViewOrders = function () {
	Component.apply(this, [false]);

	this.newSlot('preparePage');
	this.newSlot('dataReceived');

	this.detachEvent('Show', 'drawAll');
	this.bindEvent('Show', 'preparePage');

	this.loadPage = new Component.Ajax('getCurrentOrders');
	this.loadPage.bindEvent('Returned', 'dataReceived', this);
};
TechTrade.Page.ViewOrders.prototype = new Component;
TechTrade.Page.ViewOrders.prototype.dataReceived = function (data) {
	if (data == 'FU') {
		TechTrade.main.getInfo.call();
		return;
	} else if (data == 'NC') {
		return;
	}

	var toStore = [];
	var spData = data.split('\n');
	this.md5 = spData.shift();
	var line;

	this.plCache = [];
	while (line = spData.shift()) {
		line = line.split('#');
		var record = {};
		var id = line.shift();

		record.sendTo = line.shift();
		record.sendTech = line.shift();
		record.sendIssued = parseInt(line.shift(), 10);
		record.sendObeyed = parseInt(line.shift(), 10);

		record.recvFrom = line.shift();
		record.recvTech = line.shift();
		record.recvIssued = parseInt(line.shift(), 10);
		record.recvObeyed = parseInt(line.shift(), 10);

		record.name = line.join('#');

		toStore[id] = record;
		this.plCache.push(id);
	}

	this.plCache.sort(function (a,b) {
		var ra = toStore[a].name.toLowerCase(), rb = toStore[b].name.toLowerCase();
		return (ra == rb ? 0 : (ra > rb ? 1 : -1));
	});

	// Draw page
	this.data = toStore;
	this.drawAll();
};
TechTrade.Page.ViewOrders.prototype.preparePage = function () {
	this.loadPage.call(this.md5);
	this.drawAll();
};
TechTrade.Page.ViewOrders.prototype.getDocID = function () { return "page-panel"; };
TechTrade.Page.ViewOrders.prototype.draw = function () {
	var e = this.getElement();
	if (!e || TechTrade.main.page != 'VOrders') {
		return;
	}

	if (this.data) {
		e.innerHTML = '<h3>' + TechTrade.Page.ViewOrders.title + '</h3>'
			+ (this.data.length ? this.drawContents()
				: ('<p>' + TechTrade.Page.ViewOrders.noOrders + '</p>'));
	} else {
		e.innerHTML = '<h3 style="text-align:center; margin: 10px 0px">' + TechTrade.Page.subLoadingText
			+ '</h3>';
	}
};
TechTrade.Page.ViewOrders.prototype.drawContents = function () {
	var str = '<table style="width:90%;margin:0px 4%;padding:0px;border-style:none;border-width:0px;'
		+ 'border-collapse:collapse" class="list"><tr><th style="width:20%;text-align:left">'
		+ TechTrade.Page.ViewOrders.playerHdr + '</th><th style="width:40%;text-align:left">'
		+ TechTrade.Page.ViewOrders.sendHdr + '</th><th style="width:40%;text-align:left">'
		+ TechTrade.Page.ViewOrders.recvHdr + '</th></tr>';

	for (var i in this.plCache) {
		var player = this.data[this.plCache[i]];
		var text;

		str += '<tr><td style="vertical-align:top">' + player.name
			+ '</td><td style="vertical-align:top">';
		if (player.sendTo == '') {
			str += TechTrade.Page.ViewOrders.noOrders;
		} else {
			text = TechTrade.Page.ViewOrders.sendTech;
			str += text.replace(/__TI__/, player.sendTech).replace(/__TN__/,
				TechTrade.main.list[player.sendTech].name).replace(/__PN__/,
				this.data[player.sendTo].name).replace(/__OI__/,
				formatDate(player.sendIssued));
			if (player.sendObeyed) {
				str += '<br/>' + TechTrade.Page.ViewOrders.techSent.replace(/__OE__/,
					formatDate(player.sendObeyed));
			}
		}
		str += '</td><td style="vertical-align:top">'
		if (player.recvFrom == '') {
			str += TechTrade.Page.ViewOrders.noOrders;
		} else {
			text = TechTrade.Page.ViewOrders.receiveTech;
			str += text.replace(/__TI__/, player.recvTech).replace(/__TN__/,
				TechTrade.main.list[player.recvTech].name).replace(/__PN__/,
				this.data[player.recvFrom].name).replace(/__OI__/,
				formatDate(player.recvIssued));
			if (player.recvObeyed) {
				str += '<br/>' + TechTrade.Page.ViewOrders.techReceived.replace(/__OE__/,
					formatDate(player.recvObeyed));
			}
		}
		str += '</td></tr>';
	}

	str += '</table>';
	return str;
};


/* TechTrade.Page.SetOrders component
 *
 * Handles the list of current orders
 */
TechTrade.Page.SetOrders = function () {
	Component.apply(this, [false]);

	this.newEvent('TechListUpdated');
	this.newEvent('SubmitError');

	this.newSlot('preparePage');
	this.newSlot('dataReceived');
	this.newSlot('sendOrders');
	this.newSlot('ordersSent');

	this.detachEvent('Show', 'drawAll');
	this.bindEvent('Show', 'preparePage');

	this.editor = new TechTrade.OrdersEditor(this);
	this.addChild(this.editor);
	this.bindEvent('TechListUpdated', 'techListUpdated', this.editor);
	this.bindEvent('SubmitError', 'submitError', this.editor);
	this.editor.bindEvent('Submitted', 'sendOrders', this);

	this.loadPage = new Component.Ajax('getChangeOrdersData');
	this.loadPage.bindEvent('Returned', 'dataReceived', this);
	this.submitOrders = new Component.Ajax('submitOrders');
	this.submitOrders.bindEvent('Returned', 'ordersSent', this);
};
TechTrade.Page.SetOrders.prototype = new Component;
TechTrade.Page.SetOrders.prototype.dataReceived = function (data) {
	if (data == 'FU') {
		TechTrade.main.getInfo.call();
		return;
	} else if (data == 'NC') {
		return;
	}

	// Fetch the tech lists
	var spLine, nTechs;
	var spData = data.split('\n');
	var newMD5 = spData.shift();

	spLine = spData.shift().split('#');
	this.lastSet = parseInt(spLine.shift(), 10);
	this.nextSet = parseInt(spLine.shift(), 10);
	if (this.nextSet == 0) {
		var nPlayers = parseInt(spData.shift(), 10);
		var techList = [];

		for (var i = 0; i < nPlayers; i ++) {
			spLine = spData.shift().split('#');
			var pId = spLine.shift();
			var pData = {};

			pData.onVacation = (spLine.shift() == '1');
			pData.lastSubmission = parseInt(spLine.shift(), 10);
			pData.restriction = (spLine.shift() != '0');
			nTechs = parseInt(spLine.shift(), 10);
			pData.name = spLine.join('#');
			pData.techs = [];

			for (var j = 0; j < nTechs; j ++) {
				spLine = spData.shift().split('#');
				pData.techs[spLine[0]] = spLine[1];
			}

			spLine = spData.shift();
			if (spLine == '') {
				pData.requests = [];
			} else {
				pData.requests = spLine.split('#');
			}

			techList[pId] = pData;
		}

		this.techList = techList;

		// Generate the player name cache
		this.plCache = [];
		for (j in this.techList) {
			this.plCache.push(j);
		}
		var me = this;
		this.plCache.sort(function (a, b) {
			var pa = me.techList[a].name.toLowerCase(), pb = me.techList[b].name.toLowerCase();
			return (pa > pb ? 1 : (pa < pb ? -1 : 0));
		});
		this.onTechListUpdated(this.techList, this.plCache);
	} else {
		this.plCache = this.techList = null;
	}
	this.md5 = newMD5;

	// Draw page
	this.drawAll();
};
TechTrade.Page.SetOrders.prototype.sendOrders = function (orders) {
	var toSend = [];
	for (var i in orders) {
		if (! orders[i].sendTo) {
			continue;
		}
		toSend.push(orders[i].player + '#' + orders[i].sendTech + '#' + orders[i].sendTo);
	}
	this.submitOrders.call(toSend.join('!'));
};
TechTrade.Page.SetOrders.prototype.ordersSent = function (data) {
	if (data == 'FU') {
		TechTrade.main.getInfo.call();
		return;
	}

	if (data.indexOf('ERR') == 0) {
		this.onSubmitError();
		data = data.replace(/^ERR/, '');
	}
	this.dataReceived(data);
};
TechTrade.Page.SetOrders.prototype.preparePage = function () {
	this.data = null;
	this.loadPage.call(this.md5);
	this.drawAll();
};
TechTrade.Page.SetOrders.prototype.getDocID = function () { return "page-panel"; };
TechTrade.Page.SetOrders.prototype.draw = function () {
	var e = this.getElement();
	if (!e || TechTrade.main.page != 'SOrders') {
		return;
	}

	if (this.md5) {
		var str = '<h3>' + TechTrade.Page.SetOrders.title + '</h3>';
		if (this.nextSet != 0) {
			str += '<p>' + TechTrade.Page.SetOrders.alreadySubmitted.replace(/__LS__/,
				formatDate(this.lastSet)).replace(/__NS__/, formatDate(this.nextSet)) + '</p>';
		} else {
			if (this.editor.getElement()) {
				return;
			}
			str += '<div id="' + this.editor.getDocID() + '">&nbsp;</div>';
		}
		e.innerHTML = str;
	} else {
		e.innerHTML = '<h3 style="text-align:center; margin: 10px 0px">' + TechTrade.Page.subLoadingText
			+ '</h3>';
	}
};


/** TechTrade.OrdersEditor component
 *
 * Handles editing the technology trading orders
 */
TechTrade.OrdersEditor = function (page) {
	Component.apply(this, [false]);

	this.newSlot('techListUpdated');
	this.newSlot('playerChanged');
	this.newSlot('techChanged');
	this.newSlot('submitError');
	this.newEvent('Submitted');

	this.plSelector = new Component.Form.DropDown('-', '----');
	this.addChild(this.plSelector);
	this.plSelector.bindEvent('Selected', 'playerChanged', this);

	this.tSelector = new Component.Form.DropDown('-', '----');
	this.addChild(this.tSelector);
	this.tSelector.bindEvent('Selected', 'techChanged', this);

	this.page = page;
	this.orders = null;
};
TechTrade.OrdersEditor.prototype = new Component;
TechTrade.OrdersEditor.prototype.techListUpdated = function (techList, plCache) {
	this.techList = techList;
	this.dependencies = TechTrade.computeDependencies(techList);
	if (this.orders) {
		alert("FIXME: tech list received, orders should be updated");
	} else {
		this.ordersSet = 0;
		this.orders = [];
		this.rOrders = [];
		for (var i in plCache) {
			var pid = plCache[i];
			this.orders.push({
				player: pid,
				sendTech: null,
				sendTo: null,
				recvTech: null,
				recvFrom: null
			});
			this.rOrders[pid] = i;
		}
	}
	this.drawAll();
};
TechTrade.OrdersEditor.prototype.draw = function () {
	var e = this.getElement();
	if (!e || ! this.orders) {
		return;
	}

	if (this.submitting) {
		e.innerHTML = '<p>' + TechTrade.OrdersEditor.submitting + '</p>';
	} else if (this.orderEdit) {
		e.innerHTML = this.drawEditor();
	} else {
		e.innerHTML = this.drawList();
	}
};
TechTrade.OrdersEditor.prototype.drawList = function () {
	var bs0 = 'text-align:left;border-style:solid;border-color:white;border-width: 0px 0px 1px 0px';
	var bs = 'vertical-align:top;border-style:solid;border-color:white;border-width: 1px 0px;padding:10px 0px';
	var str = '<table style="width:91%;padding:0px;margin:10px 4%;border-collapse:collapse" class="list">'
		+ '<tr><th style="width:20%;' + bs0 + '">' + TechTrade.Page.ViewOrders.playerHdr
		+ '</th><th style="width:40%;' + bs0 + '">' + TechTrade.Page.ViewOrders.sendHdr
		+ '</th><th style="' + bs0 + '">' + TechTrade.Page.ViewOrders.recvHdr
		+ '</th></tr>';

	for (var i in this.orders) {
		var pOrder = this.orders[i];

		str += '<tr><td style="' + bs + '">' + this.techList[pOrder.player].name + '</td>';
		if (!this.dependencies[pOrder.player].ok) {
			str += '<td style="' + bs + ';text-align:center;color:red;font-weight:bold" colspan="2">'
				+ TechTrade.OrdersEditor.cantTrade + '</td>';
		} else {
			str += '<td style="' + bs + '">';
			if (pOrder.sendTech) {
				str += TechTrade.OrdersEditor.sendTech.replace(/__TI__/,
						pOrder.sendTech).replace(/__TN__/,
						TechTrade.main.list[pOrder.sendTech].name).replace(/__PN__/,
						this.techList[pOrder.sendTo].name)
					+ '<br/>(<a href="#" onclick="Component.list[\'' + this.id + '\'].delOrder('
					+ i + ',false);return false">' + TechTrade.OrdersEditor.delOrder
					+ '</a> - <a href="#" onclick="Component.list[\'' + this.id + '\'].editOrder('
					+ i + ',false);return false">' + TechTrade.OrdersEditor.editOrder
					+ '</a>)';
			} else {
				str += TechTrade.Page.ViewOrders.noOrders + ' (<a href="#" onclick="Component.list[\''
					+ this.id + '\'].addOrder(' + i + ',false);return false">'
					+ TechTrade.OrdersEditor.addOrder + '</a>)';
			}
			str += '</td><td style="' + bs + '">';
			if (pOrder.recvTech) {
				str += TechTrade.OrdersEditor.recvTech.replace(/__TI__/,
						pOrder.recvTech).replace(/__TN__/,
						TechTrade.main.list[pOrder.recvTech].name).replace(/__PN__/,
						this.techList[pOrder.recvFrom].name)
					+ '<br/>(<a href="#" onclick="Component.list[\'' + this.id + '\'].delOrder('
					+ i + ',true);return false">' + TechTrade.OrdersEditor.delOrder
					+ '</a> - <a href="#" onclick="Component.list[\'' + this.id + '\'].editOrder('
					+ i + ',true);return false">' + TechTrade.OrdersEditor.editOrder
					+ '</a>)';
			} else {
				str += TechTrade.Page.ViewOrders.noOrders + ' (<a href="#" onclick="Component.list[\''
					+ this.id + '\'].addOrder(' + i + ',true);return false">'
					+ TechTrade.OrdersEditor.addOrder + '</a>)';
			}
		}
		str += '</tr>';
	}
	str += '</table><p style="padding:0px;text-align:center"><input type="button" value="'
		+ TechTrade.OrdersEditor.submitOrders + '" onclick="Component.list[\'' + this.id
		+ '\'].confirmSubmit();return false" ' + (this.ordersSet ? '' : 'disabled="disabled" ')
		+ 'id="submit-orders" /></p>';

	return str;
};
TechTrade.OrdersEditor.prototype.confirmSubmit = function () {
	var e = document.getElementById('submit-orders');
	if (!e) {
		return;
	}

	e.disabled = true;
	if (confirm(TechTrade.OrdersEditor.confirmSubmit)) {
		this.submitting = true;
		this.drawAll();
		this.onSubmitted(this.orders);
	} else {
		e.disabled = false;
	}
};
TechTrade.OrdersEditor.prototype.submitError = function () {
	alert(TechTrade.OrdersEditor.submitError);
	this.submitting = false;
};
TechTrade.OrdersEditor.prototype.delOrder = function (index, isRec) {
	var order = this.orders[index];

	if (isRec) {
		var oOrder = this.orders[this.rOrders[order.recvFrom]];
		oOrder.sendTo = oOrder.sendTech = order.recvFrom = order.recvTech = null;
	} else {
		var oOrder = this.orders[this.rOrders[order.sendTo]];
		oOrder.recvFrom = oOrder.recvTech = order.sendTo = order.sendTech = null;
	}
	this.ordersSet --;
	this.drawAll();
};
TechTrade.OrdersEditor.prototype.addOrder = function (index, isRec) {
	this.initEditor({
		order: index,
		isReceive: isRec,
		oTech: null,
		oPlayer: null
	});
};
TechTrade.OrdersEditor.prototype.editOrder = function (index, isRec) {
	this.initEditor({
		order: index,
		isReceive: isRec,
		oTech: isRec ? this.orders[index].recvTech : this.orders[index].sendTech,
		oPlayer: isRec ? this.orders[index].recvFrom : this.orders[index].sendTo
	});
};
TechTrade.OrdersEditor.prototype.drawEditor = function () {
	var eParams = this.orderEdit;
	var order = this.orders[eParams.order];
	var text = eParams.isReceive ? TechTrade.OrdersEditor.willReceive : TechTrade.OrdersEditor.willSend;
	var str = '<p style="padding: 20px 50px">' + text.replace(/__PN__/,
			this.techList[order.player].name).replace(/__SP1__/,
			'<span id="' + this.tSelector.getDocID() + '">&nbsp;</span>').replace(/__SP2__/,
			'<span id="' + this.plSelector.getDocID() + '">&nbsp;</span>')
		+ ' <input type="button" value="' + TechTrade.OrdersEditor.ok + '" onclick="Component.list[\''
		+ this.id + '\'].confirmEdit();return false" ' + (eParams.confirmOk ? '' : ' disabled="disabled"')
		+ '/> <input type="button" value="' + TechTrade.OrdersEditor.cancel + '" onclick="Component.list[\''
		+ this.id + '\'].cancelEdit();return false" />'
		+ '</p>';

	return str;
};
TechTrade.OrdersEditor.prototype.cancelEdit = function () {
	this.orderEdit = null;
	this.drawAll();
};
TechTrade.OrdersEditor.prototype.confirmEdit = function () {
	if (this.orderEdit) {
		var order = this.orders[this.orderEdit.order];
		var rOrder = this.orders[this.rOrders[this.orderEdit.player]];
		if (this.orderEdit.isReceive) {
			var oOrder = this.orderEdit.oPlayer ? this.orders[this.rOrders[this.orderEdit.oPlayer]] : null;
			if (oOrder) {
				oOrder.sendTech = null;
				oOrder.sendTo = null;
			}

			order.recvTech = this.orderEdit.tech;
			order.recvFrom = this.orderEdit.player;
			rOrder.sendTech = this.orderEdit.tech;
			rOrder.sendTo = order.player;
		} else {
			var oOrder = this.orderEdit.oPlayer ? this.orders[this.rOrders[this.orderEdit.oPlayer]] : null;
			if (oOrder) {
				oOrder.recvTech = null;
				oOrder.recvFrom = null;
			}

			rOrder.recvTech = this.orderEdit.tech;
			rOrder.recvFrom = order.player;
			order.sendTech = this.orderEdit.tech;
			order.sendTo = this.orderEdit.player;
		}
		if (!this.orderEdit.oPlayer) {
			this.ordersSet ++;
		}
	}
	this.cancelEdit();
};
TechTrade.OrdersEditor.prototype.initEditor = function (editData) {
	this.orderEdit = editData;
	this.orderEdit.tech = this.orderEdit.oTech;
	this.orderEdit.player = this.orderEdit.oPlayer;
	this.updateEditorStatus();
};
TechTrade.OrdersEditor.prototype.updateEditorStatus = function () {
	this.orderEdit.confirmOk = (this.orderEdit.tech && this.orderEdit.player);

	var order = this.orders[this.orderEdit.order];
	var tPlayers = [], tTechs = [];
	var depList = this.orderEdit.isReceive ? this.dependencies[order.player].canReceive
		: this.dependencies[order.player].canSend;

	for (var i in depList) {
		var oOrder = this.orders[this.rOrders[depList[i].player]];
		if (this.orderEdit.isReceive && oOrder.sendTech && oOrder.sendTo != order.player
			|| !this.orderEdit.isReceive && oOrder.recvTech && oOrder.recvFrom != order.player) {

			continue;
		}

		if (('#' + tPlayers.join('#') + '#').indexOf('#' + depList[i].player + '#') == -1 
			&& (!this.orderEdit.tech || depList[i].tech == this.orderEdit.tech) ) {
			tPlayers.push(depList[i].player);
		}
		if (('#' + tTechs.join('#') + '#').indexOf('#' + depList[i].tech + '#') == -1
			&& (!this.orderEdit.player || depList[i].player == this.orderEdit.player) ) {
			tTechs.push(depList[i].tech);
		}
	}

	this.plSelector.clearOptions();
	this.plSelector.appendOption('', TechTrade.OrdersEditor.selectPlayer);
	for (var i in tPlayers) {
		this.plSelector.appendOption(tPlayers[i], this.techList[tPlayers[i]].name);
	}
	this.plSelector.selected = this.orderEdit.player ? this.orderEdit.player : '';

	this.tSelector.clearOptions();
	this.tSelector.appendOption('', TechTrade.OrdersEditor.selectTech);
	for (var i in tTechs) {
		this.tSelector.appendOption(tTechs[i], TechTrade.main.list[tTechs[i]].name);
	}
	this.tSelector.selected = this.orderEdit.tech ? this.orderEdit.tech : '';

	this.drawAll();
};
TechTrade.OrdersEditor.prototype.playerChanged = function (player) {
	if (!this.orderEdit) {
		return;
	}
	this.orderEdit.player = (player == '' ? null : player);
	this.updateEditorStatus();
};
TechTrade.OrdersEditor.prototype.techChanged = function (tech) {
	if (!this.orderEdit) {
		return;
	}
	this.orderEdit.tech = (tech == '' ? null : tech);
	this.updateEditorStatus();
};
