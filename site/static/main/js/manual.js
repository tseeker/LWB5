$(function(){
	$("#showcontents").click(function() {
		$("#pcontents").css("display", "block");
		$("#hidecontents").css("display", "inline");
		$("#showcontents").css("display", "none");
	});
	$("#hidecontents").click(function() {
		$("#pcontents").css("display", "none");
		$("#showcontents").css("display", "inline");
		$("#hidecontents").css("display", "none");
	});
	$("#hidecontents").css("display", "inline");
});
