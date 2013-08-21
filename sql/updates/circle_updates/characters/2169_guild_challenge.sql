DROP TABLE IF EXISTS `guild_challenge`;

CREATE TABLE `guild_challenge` (
	`guildId` INT(10) UNSIGNED NOT NULL,
	`dungeonCount` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`raidCount` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`RBGCount` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`guildId`)
)
COLLATE='utf8_general_ci'
ENGINE=InnoDB