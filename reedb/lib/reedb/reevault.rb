# ====================================================
# Copyright 2014 Random Robot Softworks (see @author)
# @author: Katharina Sabel | www.2rsoftworks.de
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at
# http://opensource.org/licenses/MIT)
# ====================================================

# => Import custom errors
require_relative 'errors/encryption_errors'

# => Import security package classes
require_relative 'security/multifish'
require_relative 'security/twofish'
require_relative 'security/aes'

module Reedb
	class ReeVault

		def initialize(name, path, encprytion)
			@path = ""
			@name = name
			@already_logging = false


			begin
				start_encryption(encprytion)
			rescue MissingEncryptionTypeError => e
			 	puts e.message
			end
			return self
		end

		def close

		end

		def start_encryption(crypt)
			crypt = 'aes' if crypt == 'auto_fill'

			if crypt == 'aes'
				@crypt = Reedb::AES.new
			elsif crypt == 'twofish'
				@crypt = Reedb::Fish.new
			elsif crypt == 'multi'
				@crypt = Reedb::MLE.new
			else
				raise MissingEncryptionTypeError.new, "Encryption failed: Missing type. Aborting..."
			end
		end

		def create(password)

			begin
				start_encryption(encprytion)
			rescue MissingUserPasswordError => e
			 	puts e.message
			end

			if password == nil
				raise MissingUserPasswordError.new, "Encryption failed: Missing user password! Aborting"

			(puts "PASSWORD CAN'T BE NIL!" ; return) if password == nil
			@name = name
			construct_path(name, path)

			if File.exists?("#{@path}/global.yml")
				puts "VAULT ALREADY EXISTS: PLEASE CHANGE NAME OR CHOOSE A DIFFERENT LOCATION!"
				init_logger
				VaultLogger.write("Vault already exists. Not overwriting!")

				# TODO: Possibly ask to load the vault instead?
				# TODO: Stop shouting at the user :)
				return
			end

			FileUtils::mkdir_p("#{@path}/data") # => Data dir
			FileUtils::mkdir("#{@path}/shasums") # => Checksum dir
			FileUtils::mkdir("#{@path}/logs") # => Logs dir

			FileUtils::chmod_R(0744, "#{@path}") #TODO: Check

			# Now that folder exists => Start logs
			init_logger

			VaultLogger.write("Vault created on #{Reepass::Utilities.get_time(false)}. All directories created successfully!")

			encrypted_key = @krypt.init_encryption("#{password}")
			hashed_pw = Robocrypt::CryptUtils.hash_base512("#{password}")
			
			config = {}
			config['vault_name'] = "#{name}"
			config['creation_date'] = "#{ReeTime.get_time}"
			config['last_updated'] = "#{ReeTime.get_time}"
			config['khash'] = "#{encrypted_key}"
			config['phash'] = "#{hashed_pw}"
			config['creation_machine'] = "#{Socket.gethostname}"
			config['updating_machine'] = "#{Socket.gethostname}"

			ReeLogger.write("Wrote key and pw hashes to config files", "info")
			config['locked'] = false

			File.open("#{@path}/global.yml", "w"){ |file| YAML.dump(config, file) }

			# => Load the vault like it existed before!
			load(name, path, "#{password}",false)

		end


		def load(password)

		end
	end
end