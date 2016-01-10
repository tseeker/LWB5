LWWidget.NewVersion = LWWidget.Page.inherits({

	setData: function (url, required) {
		this.url = url;
		this.required = required;
	},


	draw: function () {
		var _s;

		if (this.required) {
			_s = 'Due to server updates, you are required to download a new version of this widget in order to keep using it.';
		} else {
			_s = 'A new version of the LegacyWorlds widget is available!<br/><br/>'
				+ 'Upgrading the widget is not mandatory; the new version only brings new features and bugfixes.';
		}

		_s += '<br/><br/><a href="#" onclick="Base.Comp.get(' + this._cid +  ').startDownload();return false">Download the new version</a>';

		this.writeContents(_s);
	},


	startDownload: function () {
		if (window.widget) {
			widget.openURL(this.url);
		} else {
			window.open(this.url);
		}
	}

});
