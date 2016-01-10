LWWidget.GameSelector = LWWidget.Page.inherits({

	constructor: function () {
		this.base();
		this.games = [];

		this.addEvent('Select');
	},


	setGames: function (games) {
		if (games instanceof Array) {
			this.games = games;
			this.draw();
		}
	},


	draw: function () {
		var cc = '"return Base.Comp.get(' + this._cid + ').select()"',
			st = ' style="color:white;font-size:13px;background-color:#3f7fff;width:100%" ',
			st2 = ' style="color:white;font-size:13px;background-color:#3f7fff" ',
			str = '<form name="gsel" onsubmit=' + cc + '><table class="table">'
				+ '<tr><td colspan="2" style="text-align:center"><b>Game selection</b></td></tr>'
				+ '<tr><td colspan="2">&nbsp;</td></tr>'
				+ '<tr><td><label for="gsel">Game:</label></td>'
				+ '<td><select name="gsel" id="gsel"' + st;

		for (var i in this.games) {
			var g = this.games[i];
			str += '<option value="' + i + '">' + g.name + '</option>';
		}

		str += '</select></td></tr><tr><td style="text-align:right">'
			+ '<input type="checkbox" name="grem" id="grem"' + st2 + '/></td>'
			+ '<td><label for="grem">Remember selection</label></td></tr>'
			+ '<tr><td colspan="2">&nbsp;</td></tr>'
			+ '<tr><td colspan="2"><input type="submit" value="Select this game"'
			+ st + '</td></tr></table></form>';
		this.writeContents(str);

		var _cb = document.getElementById('grem');
		if (_cb) {
			_cb.checked = (LWWidget.main.getPreference('lwRemGame') == '1');
		}
	},

	select: function () {
		var _s = document.getElementById('gsel');
		if (!_s) {
			return false;
		}

		var sv = parseInt(_s.options[_s.selectedIndex].value, 10), rem = document.getElementById('grem').checked;
		LWWidget.main.setPreference('lwRemGame', rem ? '1' : '0');
		if (rem) {
			LWWidget.main.setPreference('lwGame', this.games[sv].path);
		}

		this.onSelect(this.games[sv]);

		return false;
	}

});
