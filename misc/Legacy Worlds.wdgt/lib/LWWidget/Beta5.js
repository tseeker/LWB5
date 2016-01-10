LWWidget.Beta5 = LWWidget.Game.inherits({

	constructor: function (path) {
		this.base(path);
		this.pageElement = 'b5Display';

		this.pages[0] = new LWWidget.Beta5.Player(this);
		this.pages[1] = new LWWidget.Beta5.Planets(this);
		this.pages[2] = new LWWidget.Beta5.Fleets(this);
		this.pages[3] = new LWWidget.Beta5.Cash(this);
		this.pages[4] = new LWWidget.Beta5.Techs(this);
		this.pages[5] = new LWWidget.Beta5.Msg(this);
		this.pages[6] = new LWWidget.Beta5.Forums(this);
		this.pages[7] = new LWWidget.Beta5.Ticks(this);

		this.setPage('player');

		this.addSlot('updateServerTime');
		this.stUpdate = new Base.Timer(1000, true);
		this.stUpdate.bindEvent('Tick', 'updateServerTime', this);
		this.stUpdate.start();
	},

	destroy: function () {
		this.stUpdate.destroy();
		this.base();
	},


	setData: function (data) {
		this.base(data);
		this.serverTime = parseInt(data.getAttribute('serverTime'), 10);
		this.drawServerTime();
	},


	draw: function () {
		var _e = document.getElementById('gDisplay');
		if (!_e) {
			return;
		}

		var _e2 = document.getElementById('b5Display');
		if (!_e2) {
			_e.style.overflow = 'visible';
			_e.innerHTML = '<table style="width:98%;height:208px;margin:0% 1%;padding:0%;border:0">'
				+ '<tr><td style="font-size:13px;color:white;height:16px;text-align:center" id="serverTime">&nbsp;</td></tr>'
				+ '<tr><td style="font-size:13px;color:white"><div id="b5Display" style="overflow:auto;height:192px;width:100%">&nbsp;</div></td></tr>'
				+ '</table>';
		}
		this.drawServerTime();

		this.base();
	},


	updateServerTime: function () {
		if (!this.serverTime) {
			return;
		}
		this.serverTime ++;
		this.drawServerTime();
	},

	drawServerTime: function (u) {
		var _e = document.getElementById('serverTime');
		if (!_e || !this.serverTime) {
			return;
		}

		_e.innerHTML = LWWidget.Game.formatTime(this.serverTime);
	}

});

// Register the version
LWWidget.Game.versions().put('beta5', LWWidget.Beta5);
