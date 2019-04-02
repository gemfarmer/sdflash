require 'base64'
file = File.open("basic.bin", "rb")
hex = file.read
# contents = contents.split(" ").join("")
# puts contents.inspect
str = [hex].pack('H*')
puts str
# binary = [contents].pack('H*')
# puts binary
# puts binary.unpack('H*')
encoded = Base64.encode64(str)
puts encoded
# puts 'e'.to_i(16)


# .pack("H*")
file = File.open("basic.txt", "rb")
hex = file.read
content = IO.binread(file)
puts content.inspect