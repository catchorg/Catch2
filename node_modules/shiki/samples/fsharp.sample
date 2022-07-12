type Customer(firstName, middleInitial, lastName) = 
    member this.FirstName = firstName
    member this.MiddleInitial = middleInitial
    member this.LastName = lastName

    member this.SayFullName() =
        $"{this.FirstName} {this.MiddleInitial} {this.LastName}"

let customer = Customer("Emillia", "C", "Miller")

printfn $"Hello, I'm {customer.SayFullName()}!"

// From https://dotnet.microsoft.com/languages/fsharp