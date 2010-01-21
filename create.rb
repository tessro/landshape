
STATES = [
  { :abbr => "NC", :fips => 37 }
]

shapes = ARGV.map do |arg|
  region, type, idx = arg.split(':')
  region = STATES.detect { |st| st[:abbr] == region }[:fips] if region =~ /[A-Z]{2}/
  { :region => region, :type => type, :id => idx }
end

shape_id = ARGV[1]
height = 100
klass = "#foo"

puts "["

shapes.each do |shp|
  puts `./shape-extract -t json -n#{shp[:id]} -s#{klass} -h#{height} data/tl_2009_#{shp[:region]}_#{shp[:type]}/tl_2009_#{shp[:region]}_#{shp[:type]}.shx data/tl_2009_#{shp[:region]}_#{shp[:type]}/tl_2009_#{shp[:region]}_#{shp[:type]}.shp`
  puts ","
end

puts "]"

exit

(0..793).each do |i|
  puts `./shape-extract -n#{i} -s#foo -h#{i * 100} data/tl_2009_37_zcta5/tl_2009_37_zcta5.shx data/tl_2009_37_zcta5/tl_2009_37_zcta5.shp`
end
