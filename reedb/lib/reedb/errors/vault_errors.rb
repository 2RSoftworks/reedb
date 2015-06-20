# ====================================================
# Copyright 2015 Random Robot Softworks (see @author)
# @author: Katharina Sabel | www.2rsoftworks.de
#
# Distributed under the GNU Lesser GPL Version 3
# (See accompanying LICENSE file or get a copy at
# 	https://www.gnu.org/licenses/lgpl.html)
# ====================================================

class VaultError < StandardError
end

class VaultExistsAtLocationError < VaultError
end

class VaultDoesNotExistError < VaultError
end

class VaultWritePermissionsError < VaultError
end

# I/O error or fucked?
class VaultMissingConfigurationError < VaultError
end

class FileNotFoundError < VaultError
end

class FileBusyError < VaultError
end

class MalformedSearchError < VaultError
end

# This one is VERY hypothetical
class VaultLoggerError < VaultError
end

# This has been depreciated.
class BadCacheError < VaultError
end