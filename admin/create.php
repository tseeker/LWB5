<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || $_GET['reset']) {
	$_SESSION['lw_new_game'] = array(
		"step"	=> 1
	);
}

include("cg_user_hdl_" . $_SESSION['lw_new_game']['step'] . ".inc");

include("cg_user_hdr.inc");
include("cg_user_dsp_" . $_SESSION['lw_new_game']['step'] . ".inc");

?>
 </body>
</html>
