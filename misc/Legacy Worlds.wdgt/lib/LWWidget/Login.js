LWWidget.Login = LWWidget.Page.inherits({

	constructor: function () {
		this.base();
		this.addEvent('Login');
		this.error = "";
	},

	draw: function () {
		var cc = '"return Base.Comp.get(' + this._cid + ').login()"',
			st = ' style="color:white;font-size:13px;background-color:#3f7fff;width:100%" ',
			st2 = ' style="color:white;font-size:13px;background-color:#3f7fff" ',
			str = '<form name="login" onsubmit=' + cc + '><table class="table">'
				+ '<tr><td><label for="lname">Username:</label></td><td>'
				+ '<input type="text" name="lname" id="lname" value=""'
				+ st + '/></td></tr>'
				+ '<tr><td><label for="lpass">Password:</label></td><td>'
				+ '<input type="password" name="lpass" id="lpass" value=""'
				+ st + '/></td></tr>'
				+ '<tr><td style="text-align:right"><input type="checkbox" '
				+ ' name="lrem" id="lrem"' + st2 + '/></td><td>'
				+ '<label for="lrem">Remember password</label></td></tr>'
				+ '<tr><td colspan="2">&nbsp;</td></tr>'
				+ '<tr><td colspan="2" style="text-align:center" id="lerror">&nbsp;</td></tr>'
				+ '<tr><td colspan="2">&nbsp;</td></tr>'
				+ '<tr><td colspan="2"><input type="submit" value="Connect to Legacy Worlds!"'
				+ st + '</td></tr></table></form>';
		this.writeContents(str);
		this.fillForm();
	},

	fillForm: function () {
		var l = LWWidget.main.getPreference('lwLogin'), p = LWWidget.main.getPreference('lwPassword'), r = LWWidget.main.getPreference('lwRemAuth');
		if (!document.getElementById('lname')) {
			return;
		}
		document.getElementById('lname').value = l;
		document.getElementById('lpass').value = p;
		document.getElementById('lrem').checked = (r == '1');
		document.getElementById("lerror").innerHTML = (this.error == "") ? '&nbsp;' : this.error;
	},


	setError: function (error) {
		this.error = error;
		var e = document.getElementById("lerror");
		if (e) {
			e.innerHTML = (error == "") ? '&nbsp;' : error;
		}
	},


	login: function () {
		if (!document.getElementById('lname')) {
			return false;
		}

		var l = document.getElementById('lname').value, p = document.getElementById('lpass').value,
			r = document.getElementById('lrem').checked ? '1' : '0';

		if (l == '') {
			this.setError("Please type in your username");
		} else if (p == '') {
			this.setError("Please type in your password");
		} else {
			// Save the login as a preference and store the rest until we're sure
			LWWidget.main.setPreference('lwLogin', l);
			LWWidget.main.setPreference('lwRemAuth', r);
			LWWidget.main.password = p;

			this.onLogin();
		}

		return false;
	}

});
