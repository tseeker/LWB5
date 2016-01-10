/** The Base.Util.Hashtable class allows manipulation of hashtables by encapsulating a
 * JavaScript array and using it to store the data.
 */
Base.Util.Hashtable = Base.inherits({

	/** The constructor initialises the array used to store the hashtable.
	 */
	constructor: function () {
		this.hashtable = new Array();
	},

	/** The clear() method reinitialises the hashtable. */
	clear: function () {
		this.hashtable = new Array();
	},

	/** The get() method returns the value associated with a specified key.
	 * @param	key	the key for which the value must be returned
	 * @returns		the value associated with the key or undefined/null if none was found
	 */
	get: function (key) {
		return this.hashtable[key];
	},

	/** This method indicates whether the hashtable contains items or not.
	 * @returns		true if the table is empty, false otherwise
	 */
	isEmpty: function () {
		return (parseInt(this.size()) == 0);
	},

	/** This method indicates whether the hashtable contains a specified key or not.
	 * @param	key	the key to look up in the table
	 * @return		true if a value is associated with the specified key, false otherwise
	 */
	containsKey: function (key) {
		var exists = false;
		for (var i in this.hashtable) {
			if (i == key && this.hashtable[i] != null) {
				exists = true;
				break;
			}
		}
		return exists;
	},

	/** This method checks whether a specific value is stored within the hashtable.
	 * @param	value	the value to search for
	 * @return		true if the value was found, false otherwise
	 */
	containsValue: function (value) {
		var contains = false;
		if (value != null) {
			for (var i in this.hashtable) {
				if (this.hashtable[i] == value) {
					contains=true;
					break;
				}
			}
		}
		return contains;
	},

	/** This method returns an array containing all of the hashtable's keys to which a
	 * value has been associated.
	 * @returns		the array containing all of the table's keys
	 */
	keys: function () {
		var keys=new Array();
		for (var i in this.hashtable) {
			if (this.hashtable[i] != null) {
				keys.push(i);
			}
		}
		return keys;
	},

	/** This method returns an array containing all of the values stored within the
	 * hashtable.
	 * @returns		the array containing all of the table's values
	 */
	values: function () {
		var values = new Array();
		for (var i in this.hashtable) {
			if (this.hashtable[i] != null) {
				values.push(this.hashtable[i]);
			}
		}
		return values;
	},

	/** This method stores a value in the table, associating it with the specified key.
	 * If a value was already associated with the specified key, it is overwritten.
	 * @param	key	the key to which the value is to be associated.
	 * @param	value	the value to store
	 */
	put: function (key, value) {
		if (key == null || value == null) {
			throw "NullPointerException {"+key+"},{"+value+"}";
		}
		this.hashtable[key] = value;
	},

	/** This method removes an association from the table.
	 * @param	key	the key for the association to be removed
	 */
	remove: function (key) {
		var rtn = this.hashtable[key];
		this.hashtable[key] = null;
		return rtn;
	},

	/** This method computes the amount of associations currently stored in the table.
	 * @returns		the amount of associations
	 */
	size: function () {
		var size = 0;
		for (var i in this.hashtable) {
			if (this.hashtable[i] != null) {
				size++;
			}
		}
		return size;
	}
});
