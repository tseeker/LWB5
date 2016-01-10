LWWidget.Beta5.Ticks = LWWidget.Game.Page.inherits({

	constructor: function (game) {
		this.base(game);

		this.addSlot('update');
		this.timer = new Base.Timer(1000, true);
		this.timer.bindEvent('Tick', 'update', this);
	},

	destroy: function () {
		this.timer.destroy();
		this.base();
	},


	setData: function (data) {
		var tList = data.getChildren('Ticks')[0].getChildren('Tick'), ticks = new Array();

		for (var i in tList) {
			var tick = new Array();
			tick[0] = tList[i].getText();
			tick[1] = parseInt(tList[i].getAttribute('first'), 10);
			tick[2] = parseInt(tList[i].getAttribute('interval'), 10);

			var l = tList[i].getAttribute('last');
			if (typeof l == 'string') {
				tick[3] = parseInt(l, 10);
			} else {
				tick[3] = null;
			}

			ticks.push(tick);
		}

		this.tDiff = (new Date()).getTime() - (parseInt(data.getAttribute('serverTime'), 10) * 1000);
		this.ticks = ticks;
	},


	show: function () {
		this.timer.start();
	},

	hide: function () {
		this.timer.stop();
	},


	draw: function (_e) {
		var text = new Array();

		for (var i in this.ticks) {
			text.push('Next ' + this.ticks[i][0] + ': <span id="dtick' + i + '">&nbsp;</span>');
		}

		_e.innerHTML = '<p style="text-align:center">' + text.join('<br/>') + '</p>';
		this.update();
	},


	update: function () {
		this.computeNextTicks();

		for (var i in this.ticks) {
			var t;

			if (typeof this.ticks[i][5] == 'number') {
				var d, h, m, s;

				h = this.ticks[i][5] % 86400; d = (this.ticks[i][5] - h) / 86400;
				m = h % 3600; h = (h - m) / 3600;
				s = m % 60; m = (m - s) / 60;

				if (h < 10) { h = '0' + h; }
				if (m < 10) { m = '0' + m; }
				if (s < 10) { s = '0' + s; }

				t = '<b>' + h + ':' + m + ':' + s + '</b>';
				if (d > 0) {
					t = '<b>' + d + '</b> day' + (d > 1 ? 's' : '') + ', ' + t;
				}
			} else {
				t = 'never';
			}

			var e = document.getElementById('dtick' + i);
			if (!e) {
				break;
			}
			e.innerHTML = t;
		}
	},

	computeNextTicks: function () {
		var now = Math.round(((new Date()).getTime() - this.tDiff) / 1000);
		for (var i in this.ticks) {
			if (this.ticks[i][3] && now > this.ticks[i][3]) {
				this.ticks[i][4] = null;
				continue;
			}

			if (now <= this.ticks[i][1]) {
				this.ticks[i][4] = this.ticks[i][1];
				continue;
			}

			var d = now - this.ticks[i][1], m = (d - (d % this.ticks[i][2])) / this.ticks[i][2];
			this.ticks[i][4] = this.ticks[i][1] + (m + 1) * this.ticks[i][2];
		}

		for (var i in this.ticks) {
			if (typeof this.ticks[i][4] != 'number') {
				this.ticks[i][5] = null;
			} else {
				this.ticks[i][5] = this.ticks[i][4] - now;
			}
		}
	}

});
