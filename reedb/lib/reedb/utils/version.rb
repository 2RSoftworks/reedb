# ====================================================
# Copyright 2015 Random Robot Softworks (see @author)
# @author: Katharina Sabel | www.2rsoftworks.de
#
# Distributed under the GNU Lesser GPL Version 3
# (See accompanying LICENSE file or get a copy at
# 	https://www.gnu.org/licenses/lgpl.html)
# ====================================================

require_relative 'utilities'

module Reedb
	class Version

		attr_reader :host, :timestamp, :numeral

		def initialize(existing = nil)
			if existing
				data = existing.split('_')
				@host = data[0]
				@numeral = data[1].split('.')
				@timestamp = data[2]
				return self
			end
			@host = Reedb::Utilities::parse_host
			@timestamp = DateTime.now.strftime('%Q')
			@numeral = [0, 0]
		end

		def update
			@timestamp = DateTime.now.strftime('%Q')
			@host == Reedb::Utilities::parse_host ? incr_last : incr_first
			@host = Reedb::Utilities::parse_host
		end

		# def greater(other)
		# 	if @timestamp > other.timestamp
		# 		puts "This is greater"
		# 	else
		# 		puts "Other is greater"
		# 	end
		# end

		def to_s
			return [@host, @numeral.join('.'), @timestamp].join('_')
		end

		private

		def incr_first
			@numeral[0] += 1
			@numeral[-1] = 0
		end

		def incr_last
			@numeral[-1] += 1
		end
	end
end