import logo from './logo.svg';
import './App.css';
import { useState } from 'react';
const axios = require('axios');


function App() {
  const [data, setData] = useState({k_p:0, k_d:0, k_i:0, run:false, desired: 0})
  const change_handler = e => {
    const {name, value}  = e.target
    
      setData({
          ...data,
          [name]: parseFloat(value)
      })
  }


  const start_motor = () => {
    setData({
      ...data,
      run: true
    })

    axios({
      method: 'post',
      url: 'http://localhost:4000/',
      data: data
    })
    .then((response) => {
      console.log("axios request sent fine")
    })
    .catch((error) => {
      console.log(error);
      console.log("not fine")
    });

    console.log(data)
  }

  
  const stop_motor = () => {
    setData({k_p:0, k_d:0, k_i:0, run:false, desired: 0})

    axios({
      method: 'post',
      url: 'http://localhost:4000/',
      data: data
    })
    .then((response) => {
      console.log("axios request sent fine")
    })
    .catch((error) => {
      console.log(error);
      console.log("not fine")
    });



    console.log(data)
  }

  
  

  return (
    <div className="App">
      {console.log(data)}
      <h1>PID Control Of DC Motor</h1>
      <form>
        <label><h3>K_p: <input type="number" name="k_p" onChange={change_handler} step="0.01"></input></h3></label>
        <label><h3>K_i: <input type="number" name="k_i" onChange={change_handler} step="0.01"></input></h3></label>
        <label><h3>K_d: <input type="number" name="k_d" onChange={change_handler} step="0.01"></input></h3></label>
        <label><h3>Desired Value: <input type="Number" name="desired" onChange={change_handler} step="0.01"></input></h3></label>
        <button type="button" onClick={start_motor}>Start</button>
        <button type="button" onClick={stop_motor}>Stop</button>
        <br></br>
      </form>
      <br></br>
      <br></br>
      {
        data.run == true && 
        <iframe width="400" height="300" src="http://192.168.43.168/"></iframe>
      }
    </div>
  );
}

export default App;
