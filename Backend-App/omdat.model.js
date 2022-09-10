const mongoose = require("mongoose");
const Schema = mongoose.Schema;

const dataSchema = new Schema({
    rpm: {
        type: Number
    },
});
const om2m_data= mongoose.model("data", dataSchema);

module.exports = om2m_data;
