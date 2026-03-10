# everything_is_object.rb
# In Ruby, EVERYTHING is an object — there are no primitives.
# This is fundamentally different from languages like Java, Python, or JavaScript.

puts "=== Every Value Is an Object ==="
puts

# Numbers are full objects with methods
puts "1. Methods on integer literals:"
puts "   5.class          => #{5.class}"
puts "   5.even?          => #{5.even?}"
puts "   5.between?(1,10) => #{5.between?(1, 10)}"
puts "   -42.abs          => #{-42.abs}"
puts

# You can call methods on ANY literal
puts "2. Methods on other literals:"
puts "   3.14.floor       => #{3.14.floor}"
puts "   'hello'.reverse  => #{'hello'.reverse}"
puts "   true.class       => #{true.class}"
puts "   nil.class        => #{nil.class}"
puts "   (1..5).class     => #{(1..5).class}"
puts

# nil is an object — it's the singleton instance of NilClass
puts "3. nil is a real object:"
puts "   nil.nil?         => #{nil.nil?}"
puts "   nil.to_s         => '#{nil.to_s}' (empty string!)"
puts "   nil.to_a         => #{nil.to_a.inspect} (empty array!)"
puts "   nil.to_i         => #{nil.to_i} (zero!)"
puts "   nil.object_id    => #{nil.object_id}"
puts

# Integers have iterator methods built in
puts "4. Integer iteration (5.times):"
print "   "
5.times { |i| print "#{i} " }
puts
puts

# You can ask any object for its methods
puts "5. Even integers have LOTS of methods:"
puts "   42.methods.count => #{42.methods.count}"
puts "   Sample: #{42.methods.sort.first(8).inspect}"
puts

# object_id reveals how Ruby stores small integers (Fixnum optimization)
# Small integers have a predictable object_id: (2n + 1)
puts "6. Small integer object_id pattern (2n+1):"
(0..5).each do |n|
  puts "   #{n}.object_id = #{n.object_id}  (expected: #{2 * n + 1})"
end
puts

# true, false, nil have fixed object_ids
puts "7. Singleton object IDs:"
puts "   false.object_id  => #{false.object_id}"
puts "   true.object_id   => #{true.object_id}"
puts "   nil.object_id    => #{nil.object_id}"
puts

# Classes are also objects (instances of Class)
puts "8. Classes are objects too:"
puts "   String.class     => #{String.class}"
puts "   Class.class      => #{Class.class}"
puts "   Class.superclass => #{Class.superclass}"
puts "   Module.class     => #{Module.class}"
# The circular nature: Class is an instance of Class!
puts "   Class is an instance of Class: #{Class.is_a?(Class)}"
puts "   Object is an instance of Class: #{Object.is_a?(Class)}"
