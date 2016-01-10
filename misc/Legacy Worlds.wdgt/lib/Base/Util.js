/** The Base.Util class provide a few utility functions such as number formatting. */
Base.Util = Base.inherits(null, {

	/** This class property is used on IE to determine which type of ActiveX object
	 * the current version uses, depending on the amount of frogs that fell from the
	 * sky and the amount of hairs under my armpits at this time.
	 */
	_ieXMLParser: null,

	/** The formatNumber() method transforms an integer into a string in which
	 * the integer has commas between each group of thousands.
	 * @param	n	the value to be formatted
	 */
	formatNumber: function (n) {
		var x = parseInt(n, 10);
		if (isNaN(x)) {
			return "N/A";
		}

		if (x < 0) {
			x = -x;
			s = '-';
		} else {
			s = '';
		}
		x = x.toString();

		var l = x.length, m = l % 3, c = (l - m) / 3, v = ((m > 0) ? x.substr(0, m) : ''), z;
		for (z=0;z<c;z++) {
			if (v != "" && v != '-') {
				v += ',';
			}
			v += x.substr(m+(z*3), 3);
		}
		return s + v;
	},


	/** This method parses an XML document using the brower-supplied DOM parser.
	 * @param	text	the XML data to be parsed
	 * @returns	the boolean value "false" on failure or the DOM document on
	 *			success
	 */
	parseXML: function (text) {
		var xmlDoc;
		try {
			if (Base.Browser.get().ie) {
				if (!this._ieXMLParser) {
					this._checkIEXMLParser();
				}

				xmlDoc = new ActiveXObject(this._ieXMLParser);
				xmlDoc.async = "false";
				xmlDoc.loadXML(text);
			} else {
				var parser = new DOMParser();
				xmlDoc = parser.parseFromString(text, "text/xml");
			}
		} catch (e) {
			Base.Log.write('Base.Util.parseXML(): failed with exception ' + (e.toString ? e.toString() : e));
			xmlDoc = false;
		}

		return xmlDoc;
	},


	/** This class method is used by the parseXML() method to check on known XML parsers
	 * when running IE. It throws a random exception if none of the listed implementations
	 * actually exist.
	 */
	_checkIEXMLParser: function () {
		var impls = [
			"Msxml2.DOMDocument.7.0", "Msxml2.DOMDocument.6.0",
			"Msxml2.DOMDocument.5.0", "Msxml2.DOMDocument.4.0",
			"Msxml2.DOMDocument.3.0", "MSXML2.DOMDocument",
			"MSXML.DOMDocument", "Microsoft.XMLDOM"
		];

		for (var i in impls) {
			try {
				new ActiveXObject(impls[i]);
			} catch (e) {
				continue;
			}
			this._ieXMLParser = impls[i];
			break;
		}

		if (!this._ieXMLParser) {
			throw "IEWithoutXMLParser";
		}
	}

});
