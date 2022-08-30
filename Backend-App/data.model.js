//sample mongodb model
const mongoose = require("mongoose");
const Schema = mongoose.Schema;

const dataSchema = new Schema({
    k_p: {
        type: Number
    },
    
    k_i:{
        type: Number
    },
    
    k_d:{
        type: Number
    },

    desired:{
        type: Number
    },
    
    run: {
        type: Boolean
    },
});
const Data = mongoose.model("data", dataSchema);

module.exports = Data;
