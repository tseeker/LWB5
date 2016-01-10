LWWidget.Data.Loader = Base.Comp.inherits({

	constructor: function (login, password) {
		this.base();

		this.addEvent('CommError');
		this.addEvent('FatalError');
		this.addEvent('Kick');
		this.addEvent('Load');
		this.addEvent('LoginFailure');
		this.addEvent('Maintenance');

		this.addSlot('loaderDestroyed');
		this.addSlot('dataReceived');

		this.loader = new Base.XMLLoader('', true);
		this.loader.bindEvent('Destroy', 'loaderDestroyed', this);
		this.loader.bindEvent('NetworkError', 'onCommError', this);
		this.loader.bindEvent('ServerError', 'onCommError', this);
		this.loader.bindEvent('Timeout', 'onCommError', this);
		this.loader.bindEvent('Unsupported', 'onCommError', this);
		this.loader.bindEvent('Aborted', 'onCommError', this);
		this.loader.bindEvent('Loaded', 'dataReceived', this);

		this.params = new Base.Util.Hashtable();
		this.params.put('__s', '1');
		this.params.put('__l', login);
		this.params.put('__p', password);
	},

	destroy: function () {
		if (this.loader) {
			this.loader.destroy();
		}
		this.base();
	},


	load: function (from) {
		if (this.loader.isLoading) {
			return false;
		}
		this.loader.changeParameters(LWWidget.base + '/index.php/' + from + '.xml');
		this.loader.load(this.params);
	},


	loaderDestroyed: function () {
		this.loader = null;
	},


	dataReceived: function (data) {
		var _xml = Base.Util.parseXML(data);
		if (!_xml) {
			this.onCommError();
			return;
		}

		var _doc = _xml.documentElement;
		if (_doc.nodeName == 'Maintenance') {
			var _r, _u, _c;
			_r = LWWidget.Data.Loader.getXMLText(_doc, 'Reason');
			_u = LWWidget.Data.Loader.getXMLText(_doc, 'Until');
			_c = LWWidget.Data.Loader.getXMLText(_doc, 'Current');
			this.onMaintenance(_r, _u, _c);
		} else if (_doc.nodeName == 'FatalError') {
			this.onFatalError(_doc.getAttribute('code'), LWWidget.Data.Loader.getXMLText(_doc, 'Text'));
		} else if (_doc.nodeName == 'Kicked') {
			this.onKick(LWWidget.Data.Loader.getXMLText(_doc, 'Reason'));
		} else if (_doc.nodeName == 'Failed') {
			this.onLoginFailure(_doc.getAttribute('code'));
		} else {
			var _data = LWWidget.Data.Loader.parse(_doc);
			if (_data) {
				this.onLoad(_data);
			} else {
				this.onCommError();
			}
		}
	}

}, {

	getXMLText: function (node, name) {
		var _t = '';
		for (var i=0;i<node.childNodes.length;i++) {
			var _c = node.childNodes[i];
			if (_c.nodeType == 1 && _c.nodeName == name) {
				_t += _c.childNodes[0].nodeValue;
			}
		}
		return _t;
	},

	parse: function (xmlNode) {
		if (xmlNode.nodeType != 1 || xmlNode.nodeName != 'Node') {
			return null;
		}

		var _in = (xmlNode.getAttribute('node') == '1'), _n;
		if (_in) {
			_n = new LWWidget.Data.Node(xmlNode.getAttribute('name'));
		} else {
			_n = new LWWidget.Data.Leaf(xmlNode.getAttribute('name'), LWWidget.Data.Loader.getXMLText(xmlNode, 'Text'));
		}

		for (var i=0;i<xmlNode.childNodes.length;i++) {
			var _c = xmlNode.childNodes[i];
			if (_c.nodeType != 1) {
				continue;
			}

			if (_c.nodeName == 'Attr') {
				var _t;
				if (_c.childNodes.length) {
					_t = _c.childNodes[0].nodeValue;
				} else {
					_t = '';
				}
				_n.setAttribute(_c.getAttribute('name'), _t);
			} else if (_c.nodeName == 'Node' && _in) {
				_n.addChild(this.parse(_c));
			}
		}

		return _n;
	}

});
