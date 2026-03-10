# method_missing_magic.rb
# Ruby's method_missing is one of its most powerful metaprogramming features.
# When you call a method that doesn't exist, Ruby calls method_missing instead
# of immediately raising NoMethodError. This enables "ghost methods."

puts "=== method_missing Magic ==="
puts

# --- Basic method_missing ---
puts "1. BASIC method_missing — intercept undefined methods:"

class GhostClass
  def method_missing(name, *args)
    if name.to_s.start_with?("say_")
      word = name.to_s.sub("say_", "")
      puts "   Ghost says: #{word}!"
    else
      super  # Important: call super for methods you don't handle
    end
  end

  # Always implement respond_to_missing? alongside method_missing!
  def respond_to_missing?(name, include_private = false)
    name.to_s.start_with?("say_") || super
  end
end

ghost = GhostClass.new
ghost.say_hello
ghost.say_ruby
puts "   ghost.respond_to?(:say_anything) => #{ghost.respond_to?(:say_anything)}"
puts "   ghost.respond_to?(:unknown)      => #{ghost.respond_to?(:unknown)}"
puts

# --- Dynamic attribute access (like OpenStruct) ---
puts "2. DYNAMIC ATTRIBUTES — Build your own OpenStruct:"

class FlexObject
  def initialize
    @attributes = {}
  end

  def method_missing(name, *args)
    attr_name = name.to_s
    if attr_name.end_with?("=")
      @attributes[attr_name.chomp("=")] = args.first
    elsif @attributes.key?(attr_name)
      @attributes[attr_name]
    else
      super
    end
  end

  def respond_to_missing?(name, include_private = false)
    attr_name = name.to_s.chomp("=")
    @attributes.key?(attr_name) || name.to_s.end_with?("=") || super
  end
end

obj = FlexObject.new
obj.name = "Ruby"
obj.version = 3.3
puts "   obj.name    => #{obj.name}"
puts "   obj.version => #{obj.version}"
puts

# --- Method missing for a fluent DSL ---
puts "3. FLUENT DSL — Chaining ghost methods:"

class QueryBuilder
  attr_reader :conditions

  def initialize
    @conditions = []
  end

  def method_missing(name, *args)
    if name.to_s.start_with?("where_")
      field = name.to_s.sub("where_", "")
      @conditions << "#{field} = '#{args.first}'"
      self  # Return self to allow chaining!
    else
      super
    end
  end

  def respond_to_missing?(name, include_private = false)
    name.to_s.start_with?("where_") || super
  end

  def to_s
    "SELECT * WHERE #{@conditions.join(' AND ')}"
  end
end

query = QueryBuilder.new
  .where_name("Alice")
  .where_age("30")
  .where_city("Tokyo")
puts "   #{query}"
puts

# --- The danger: forgetting super ---
puts "4. THE DANGER — What happens when you forget super:"

class BlackHole
  def method_missing(name, *args)
    # Swallows ALL method calls silently — very dangerous!
    nil
  end
end

bh = BlackHole.new
puts "   bh.anything      => #{bh.anything.inspect}"
puts "   bh.fdsjkfdsjk    => #{bh.fdsjkfdsjk.inspect}"
puts "   bh.to_s          => #{bh.to_s.inspect}"  # Even overrides to_s behavior!
puts "   ^ This breaks Ruby's object protocol. Never do this!"
puts

# --- Performance: define_method to cache ghost methods ---
puts "5. OPTIMIZATION — Cache ghost methods with define_method:"

class SmartGhost
  def method_missing(name, *args)
    if name.to_s =~ /^compute_(\w+)/
      # Define the method so next call skips method_missing
      self.class.define_method(name) do
        "computed_#{$1}"
      end
      send(name)  # Call the newly defined method
    else
      super
    end
  end

  def respond_to_missing?(name, include_private = false)
    name.to_s.start_with?("compute_") || super
  end
end

sg = SmartGhost.new
puts "   First call (via method_missing):"
puts "   sg.compute_foo => #{sg.compute_foo}"
puts "   Method now exists? #{sg.respond_to?(:compute_foo)}"
puts "   Second call (direct, no method_missing):"
puts "   sg.compute_foo => #{sg.compute_foo}"
