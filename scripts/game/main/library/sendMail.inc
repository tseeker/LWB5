<?php

class main_sendMail {

	function main_sendMail($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($file, $to, $vars) {
		if (! config::$main['sendmails']) {
			return true;
		}

		// Read the file
		$mail = @file_get_contents("{$this->lib->game->dir}/mail/$file");
		if (is_bool($mail)) {
			logText("Mail '$file' to $to failed: file not found", LOG_WARNING);
			return false;
		}

		// Generate the substitution arrays from the variables
		$vals = array_values($vars);
		$subst = array();
		foreach (array_keys($vars) as $name) {
			array_push($subst, "/_{$name}_/");
		}

		$mail = preg_replace($subst, $vals, $mail);
		$tmp = explode("\n", $mail);
		$subject = array_shift($tmp);
		$mail = join("\n", $tmp);

		$header = "From: webmaster@legacyworlds.com\r\n"
			. "Reply-To: webmaster@legacyworlds.com\r\n"
			. "X-Mailer: LegacyWorlds\r\n"
			. "Mime-Version: 1.0\r\n"
			. "Content-Type: text/plain; charset=iso-8859-1";
		if (!mail($to, $subject, $mail, $header)) {
			logText("Mail '$file' to $to failed: unable to send", LOG_WARNING);
			return false;
		}

		return true;
	}
}

?>
