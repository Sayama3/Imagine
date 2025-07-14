

time = 0

function Update(ts)
    
    time = time + ts;

    if time > 1 then
        print("Curent ts is "..math.floor(1/ts).." fps");
        time = 0
    end

end
